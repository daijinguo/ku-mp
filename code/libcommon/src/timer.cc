#include "libcommon/timer.h"

#include "uv.h"
#include <stdlib.h>

KU_NS_LEFT
namespace common
{

timer::timer(std::function<void()> &&cb, loop *p) : callback_(std::move(cb)), started_(false)
{
	timer_ = calloc(sizeof(uv_timer_t), 1);
	uv_handle_set_data((uv_handle_t *)timer_, this);

	uv_loop_t *loop = nullptr;
	if (p == nullptr || p->get_loop() == nullptr)
		loop = uv_default_loop();
	else
		loop = (uv_loop_t *)p->get_loop();

	uv_timer_init(loop, (uv_timer_t *)timer_);
}

timer::~timer()
{
	if (started_) {
		uv_timer_stop((uv_timer_t *)timer_);
	}

	if (timer_ != nullptr) {
		uv_handle_set_data((uv_handle_t *)timer_, nullptr);
		uv_close((uv_handle_t *)timer_, [](uv_handle_t *handle) { free(handle); });
	}
}

bool timer::start(uint64_t timeout, uint64_t repeat)
{
	if (!started_) {
		started_ = uv_timer_start((uv_timer_t *)timer_, (uv_timer_cb)timer_callback, timeout, repeat) == 0;
	}
	return started_;
}

bool timer::stop()
{
	if (started_ && uv_timer_stop((uv_timer_t *)timer_) == 0)
		started_ = false;
	return !started_;
}

void timer::timer_callback(void *handle)
{
	auto *p = (timer *)uv_handle_get_data((uv_handle_t *)handle);
	if (p->callback_) {
		std::function<void()> cb = p->callback_;
		p->callback_ = nullptr;

		cb();
	}
}

} // namespace common
KU_NS_RIGHT
