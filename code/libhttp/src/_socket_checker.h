#pragma once

#include "libcommon/common.h"
#include "libcommon/reference.h"
#include "uv.h"

#include <string>
#include <unordered_map>
#include <memory>

KU_NS_LEFT
namespace http
{

class _socket_checker {
	REFERENCE_COUNT(_socket_checker)

public:
	_socket_checker(uv_stream_t *socket, std::string key,
					std::shared_ptr<std::unordered_multimap<std::string, _socket_checker *> > socket_cache);
	~_socket_checker();

	int start();
	uv_stream_t *stop();

protected:
	void on_read(ssize_t status);

	static void on_alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf);
	static void on_read_cb(uv_stream_t *socket, ssize_t nread, const uv_buf_t *buf);

private:
	uv_stream_t *socket_;
	std::string key_;
	std::shared_ptr<std::unordered_multimap<std::string, _socket_checker *> > socket_cache_;
};

//
// inline functions
//

inline void _socket_checker::on_read(ssize_t status)
{
	release();
}

} // namespace http
KU_NS_RIGHT
