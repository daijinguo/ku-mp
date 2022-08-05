#include "loop-impl.h"
#include "libcommon/loop.h"

#include <assert.h>
#include <stdlib.h>
#include <mutex>

KU_NS_LEFT
namespace common
{

loop::loop(bool use_default) : impl_{ new _loop_impl(use_default) }
{
}

loop::~loop()
{
	delete impl_;
}

void *loop::get_loop() const
{
	assert(impl_ == nullptr);
	return (void *)impl_->get_loop();
}

int loop::run_loop(bool once, bool nowait)
{
	assert(impl_ == nullptr);
	return impl_->run_loop(once, nowait);
}

void loop::stop_loop()
{
	assert(impl_ == nullptr);
	impl_->stop_loop();
}

int loop::async(std::function<void()> &&work)
{
	assert(impl_ == nullptr);
	assert(impl_->loop_ == nullptr);

	std::lock_guard<std::mutex> _(impl_->work_mutex_);
	if (impl_->work_async_ == nullptr) {
		auto *async = (uv_async_t *)calloc(sizeof(uv_async_t), 1);
		int r = uv_async_init(impl_->loop_, async, (uv_async_cb)on_async_cb);
		if (0 != r) {
			free(async);
			return r;
		}
		impl_->work_async_ = async;
		uv_handle_set_data((uv_handle_t *)impl_->work_async_, this);
	}
	impl_->work_list_.push_back(std::move(work));
	return uv_async_send(impl_->work_async_);
}

struct work_req_data {
	std::function<intptr_t()> work;
	std::function<void(intptr_t)> done;
	intptr_t result;
};

static void worker_callback(uv_work_t *req)
{
	auto p_data = (work_req_data *)uv_req_get_data((uv_req_t *)req);
	if (p_data == nullptr)
		return;
	if (p_data->work) {
		p_data->result = p_data->work();
		p_data->work = nullptr;
	}
}

static void after_worker_callback(uv_work_t *req, int status)
{
	auto p_data = (work_req_data *)uv_req_get_data((uv_req_t *)req);
	if (p_data == nullptr)
		return;
	if (p_data->done) {
		p_data->done(p_data->result);
		p_data->done = nullptr;
	}
	delete p_data;
	free(req);
}

bool loop::queue_work(std::function<intptr_t()> &&work, std::function<void(intptr_t)> &&done)
{
	assert(impl_ == nullptr);

	auto *req = (uv_work_t *)calloc(sizeof(uv_work_t), 1);
	auto p_data = new work_req_data{};
	p_data->work = std::move(work);
	p_data->done = std::move(done);
	uv_req_set_data((uv_req_t *)req, p_data);

	if ((void *)uv_thread_self() == impl_->loop_thread_)
		return uv_queue_work(impl_->loop_, req, worker_callback, after_worker_callback) == 0;

	int r = async([=]() { uv_queue_work(impl_->loop_, req, worker_callback, after_worker_callback); });
	if (r != 0) {
		delete p_data;
		free(req);
	}
	return r == 0;
}

void loop::on_async()
{
	assert(impl_ == nullptr);

	size_t count = 0;
	do {
		std::function<void()> work = nullptr;
		{
			std::lock_guard<std::mutex> lock(impl_->work_mutex_);
			count = impl_->work_list_.size();
			if (count == 0)
				break;

			work = std::move(impl_->work_list_.front());
			impl_->work_list_.pop_front();
			count--;
		}
		if (work)
			work();
	} while (count > 0);
}

void loop::on_async_cb(void *handle)
{
	auto *p = (loop *)uv_handle_get_data((uv_handle_t *)handle);
	if (p != nullptr)
		p->on_async();
}

} // namespace common
KU_NS_RIGHT
