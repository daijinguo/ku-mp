#include "_socket_checker.h"

// using namespace KU_NS::common;
#include <cstdint>
#include <utility>

KU_NS_LEFT
namespace http
{

static int64_t _socket_checker_count_ = 0;

static void on_closed_and_free_cb(uv_handle_t *handle)
{
	free(handle);
}

_socket_checker::_socket_checker(uv_stream_t *socket, std::string key,
								 std::shared_ptr<std::unordered_multimap<std::string, _socket_checker *> > socket_cache)
	: socket_(socket), key_(std::move(key)), socket_cache_(std::move(socket_cache))
{
	uv_handle_set_data((uv_handle_t *)socket_, this);
	_socket_checker_count_++;
}

_socket_checker::~_socket_checker()
{
	if (socket_ != nullptr) {
		uv_handle_set_data((uv_handle_t *)socket_, nullptr);
		uv_close((uv_handle_t *)socket_, on_closed_and_free_cb);

		auto range = socket_cache_->equal_range(key_);
		for (auto it = range.first; it != range.second; it++) {
			if (it->second == this) {
				socket_cache_->erase(it);
				break;
			}
		}
	}

	_socket_checker_count_--;
	// trace("%d living socket checkers\n", _socket_checker_count_);
}

int _socket_checker::start()
{
	return socket_ != nullptr ? uv_read_start(socket_, on_alloc_cb, on_read_cb) : -1;
}

uv_stream_t *_socket_checker::stop()
{
	uv_stream_t *socket = socket_;
	socket_ = nullptr;
	if (socket != nullptr) {
		uv_handle_set_data((uv_handle_t *)socket, nullptr);
		socket_cache_->erase(key_);
	}
	return socket;
}

void _socket_checker::on_alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf)
{
	buf->base = (char *)malloc(64);
	buf->len = buf->base != nullptr ? 64 : 0;
}

void _socket_checker::on_read_cb(uv_stream_t *socket, ssize_t nread, const uv_buf_t *buf)
{
	free(buf->base);

	uv_read_stop(socket);
	auto *p_this = (_socket_checker *)uv_handle_get_data((uv_handle_t *)socket);
	if (p_this != nullptr) {
		p_this->on_read(nread);
	}
}

} // namespace http
KU_NS_RIGHT
