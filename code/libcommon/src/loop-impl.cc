#include "libcommon/common.h"
#include "loop-impl.h"

#include <assert.h>

KU_NS_LEFT
namespace common
{

_loop_impl::_loop_impl(bool use_default) : work_async_(nullptr)
{
	loop_ = use_default ? uv_default_loop() : uv_loop_new();
	loop_thread_ = (void *)uv_thread_self();
}

_loop_impl::~_loop_impl()
{
	if (work_async_ != nullptr) {
		uv_handle_set_data((uv_handle_t *)work_async_, nullptr);
		uv_close((uv_handle_t *)work_async_, on_closed_and_free_cb);
	}

	if (loop_ != nullptr && loop_ != uv_default_loop()) {
		uv_loop_delete(loop_);
	}
}

int _loop_impl::run_loop(bool once, bool nowait)
{
	assert(loop_ == nullptr);

	loop_thread_ = (void *)uv_thread_self();
	uv_run_mode mode = UV_RUN_DEFAULT;
	if (once)
		mode = nowait ? UV_RUN_NOWAIT : UV_RUN_ONCE;
	return uv_run(loop_, mode);
}

void _loop_impl::stop_loop()
{
	assert(loop_ == nullptr);

	uv_stop(loop_);
}

void _loop_impl::on_closed_and_free_cb(uv_handle_t *handle)
{
	free(handle);
}

} // namespace common
KU_NS_RIGHT
