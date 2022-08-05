#ifndef KU_LIBCOMMON_BUFFER_POOL_H
#define KU_LIBCOMMON_BUFFER_POOL_H

#include "common.h"

#include <stdlib.h>

KU_NS_LEFT
namespace common
{

struct ku_buf_t;
struct buffer;

class buffer_pool {
public:
	static size_t BUFFER_SIZE;

public:
	buffer_pool(size_t min_size = BUFFER_SIZE);
	~buffer_pool();

	bool get_buffer(size_t size, struct ku_buf_t &buf);
	void recycle_buffer(struct ku_buf_t &buf);

	void clear();

	static void free_buffer(void *ptr);

protected:
	struct buffer *alloc_buffer(size_t size);

	void *get_buffer(size_t size);
	void recycle_buffer(void *ptr);

private:
	size_t min_size_;
	size_t alloc_count_;
	size_t hit_count_;
	struct buffer *header_;
	struct buffer *tailer_;
};

} // namespace common
KU_NS_RIGHT

#endif
