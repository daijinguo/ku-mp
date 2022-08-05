#include "libcommon/buffer-pool.h"

#include "uv.h"

KU_NS_LEFT
namespace common
{

#define BUFFER_CACHE_ENABLE 1

struct ku_buf_t : public uv_buf_t {};

struct buffer {
	size_t size;
	struct buffer *next;
};

size_t buffer_pool::BUFFER_SIZE = 256 * 1024;

buffer_pool::buffer_pool(size_t min_size)
	: min_size_(min_size), alloc_count_(0), hit_count_(0), header_(nullptr), tailer_(nullptr)
{
}

buffer_pool::~buffer_pool()
{
	clear();
}

void buffer_pool::clear()
{
	while (header_ != tailer_) {
		buffer *next = header_->next;
		free(header_);
		header_ = next;
	}
	tailer_ = nullptr;
}

bool buffer_pool::get_buffer(size_t size, struct ku_buf_t &buf)
{
	buf.base = (char *)get_buffer(size);
	if (buf.base == nullptr)
		return false;

#if (BUFFER_CACHE_ENABLE == 1)
	auto *p_buf = (buffer *)(buf.base - sizeof(buffer));
	buf.len = static_cast<decltype(buf.len)>(p_buf->size > size ? p_buf->size : size);
#else
	buf.len = size;
#endif
	return true;
}

void *buffer_pool::get_buffer(size_t size)
{
#if (BUFFER_CACHE_ENABLE == 1)
	buffer *p_buf;
	if (header_ != nullptr) {
		p_buf = header_;
		header_ = header_->next;
		if (header_ == nullptr)
			tailer_ = nullptr;
		hit_count_++;
	} else {
		p_buf = alloc_buffer(size);
		if (p_buf == nullptr)
			return nullptr;
		alloc_count_++;
	}

	p_buf->next = nullptr;
	return (char *)p_buf + sizeof(buffer);
#else
	return (char *)malloc(size);
#endif
}

void buffer_pool::recycle_buffer(void *ptr)
{
	if (ptr == nullptr)
		return;

#if (BUFFER_CACHE_ENABLE == 1)
	auto *p_buf = (buffer *)((char *)ptr - sizeof(buffer));
	if (tailer_ == nullptr) {
		header_ = tailer_ = p_buf;
	} else {
		tailer_->next = p_buf;
		tailer_ = p_buf;
	}
#else
	free(ptr);
#endif
}

void buffer_pool::recycle_buffer(struct ku_buf_t &buf)
{
	if (buf.base == nullptr)
		return;

	recycle_buffer(buf.base);
	buf.base = nullptr;
	buf.len = 0;
}

struct buffer *buffer_pool::alloc_buffer(size_t size)
{
	if (size < min_size_)
		size = min_size_;

	auto *buf = (buffer *)malloc(sizeof(buffer) + size);
	if (buf == nullptr)
		return nullptr;

	buf->size = size;
	buf->next = nullptr;
	return buf;
}

void buffer_pool::free_buffer(void *ptr)
{
	// ptr must be alloced by a buffer_pool
	auto *p_buf = (buffer *)((char *)ptr - sizeof(buffer));
	free(p_buf);
}

} // namespace common
KU_NS_RIGHT
