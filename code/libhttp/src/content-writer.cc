#include "libcommon/buffer-pool.h"
#include "libhttp/content-writer.h"
#include "uv.h"

#include <assert.h>
#include <stdlib.h>

using namespace KU_NS::common;

KU_NS_LEFT
namespace http
{

struct write_req : public uv_write_t {
	uv_buf_t buf;
	content_done done;

	write_req(const char *data = nullptr, size_t size = 0, content_done d = nullptr)
	{
		buf.base = const_cast<char *>(data);
		buf.len = static_cast<decltype(buf.len)>(size);
		done = d;
	}

	~write_req()
	{
		if (done) {
			done();
		}
	}
};

static void on_closed_and_free_cb(uv_handle_t *handle)
{
	free(handle);
}

content_writer::content_writer(loop *lp) : loop_(lp), socket_(nullptr), content_written_(0), content_to_write_(0)
{
	content_sink_ = [this](const char *data, size_t size, content_done done) {
		auto req = std::make_shared<write_req>(data, size, done);
		if (!headers_written_ || writing_req_ || last_socket_error_ < 0) {
			// push to list tail, will be written in on_written_cb()
			req_list_.push_back(req);
			return;
		}

		if (!req_list_.empty()) {
			// push to list tail, pop the front
			req_list_.push_back(req);
			req = req_list_.front();
			req_list_.pop_front();
		}

		int r = (int)req->buf.len;
		if (r > 0)
			r = write_content(req);
		if (r < 0) {
			// to stop write
			// trace("%p:%p stop: %s\n", this, socket_, uv_err_name(r));
			if (writing_req_)
				on_written_cb(writing_req_.get(), r);
			else
				on_write_end(r);
		}
	};
}

content_writer::~content_writer()
{
	assert(!writing_req_);
	if (writing_req_) {
		uv_req_set_data((uv_req_t *)writing_req_.get(), nullptr);
	}

	req_list_.clear();

	auto *tcp = (uv_stream_t *)socket_;
	socket_ = nullptr;
	if (tcp != nullptr) {
		uv_handle_set_data((uv_handle_t *)tcp, nullptr);
		uv_close((uv_handle_t *)tcp, on_closed_and_free_cb);
	}
}

int content_writer::start_write(std::shared_ptr<std::string> headers, content_provider provider)
{
	assert(!writing_req_);
	assert(req_list_.empty());
	req_list_.clear();

	content_provider_ = provider;
	headers_written_ = false;

	if (provider && content_to_write_ <= buffer_pool::BUFFER_SIZE) {
		// preload small content and combine to single buffer
		provider(content_written_, content_to_write_, content_sink_);
		for (const auto &req : req_list_) {
			headers->append(req->buf.base, req->buf.len);
			content_written_ += req->buf.len;
		}
		req_list_.clear();
	}

	writing_req_ = std::make_shared<write_req>(headers->c_str(), headers->size(), [headers]() {});
	uv_req_set_data((uv_req_t *)writing_req_.get(), this);

	int r = uv_write(writing_req_.get(), (uv_stream_t *)socket_, &writing_req_->buf, 1, (uv_write_cb)on_written_cb);
	if (r < 0) {
		writing_req_.reset();
		return r;
	}

	headers_written_ = true;
	prepare_next();
	return 0;
}

void content_writer::prepare_next()
{
	if (content_provider_ && !is_write_done())
		content_provider_(content_written_, content_to_write_, content_sink_);
	else
		set_write_done();
}

int content_writer::write_content(std::shared_ptr<write_req> req)
{
	int64_t max_write = content_to_write_ - content_written_;
	if (req->buf.len > max_write)
		req->buf.len = static_cast<decltype(req->buf.len)>(max_write);
	content_written_ += req->buf.len;

	assert(!writing_req_);
	writing_req_ = req;
	uv_req_set_data((uv_req_t *)writing_req_.get(), this);

	int r = uv_write(writing_req_.get(), (uv_stream_t *)socket_, &writing_req_->buf, 1, (uv_write_cb)on_written_cb);
	if (r < 0) {
		writing_req_.reset();
	}
	return r;
}

int content_writer::write_next()
{
	if (is_write_done())
		return 0;

	if (req_list_.empty()) {
		prepare_next();
		return 0;
	}

	auto req = req_list_.front();
	req_list_.pop_front();

	int r = (int)req->buf.len;
	if (r >= 0) {
		r = write_content(req);
	}

	if (r >= 0 && req_list_.empty()) {
		prepare_next();
	}
	return r;
}

void content_writer::on_written_cb(void *req, int status)
{
	auto *p_this = (content_writer *)uv_req_get_data((uv_req_t *)req);
	if (p_this == nullptr)
		return;

	p_this->last_socket_error_ = status;
	p_this->writing_req_.reset();

	if (p_this->is_write_done()) {
		p_this->content_provider_ = nullptr;
		p_this->on_write_end(status);
		return;
	}

	if (status >= 0) {
		status = p_this->write_next();
		if (status < 0) {
			// trace("%p:%p write_socket: %s\n", p_this, p_this->socket_, uv_err_name(status));
		}
	} else {
		// trace("%p:%p on_written_cb: %s\n", p_this, p_this->socket_, uv_err_name(status));
	}
	if (status < 0) {
		p_this->content_provider_ = nullptr;
		p_this->on_write_end(status);
	}
}

} // namespace http
KU_NS_RIGHT
