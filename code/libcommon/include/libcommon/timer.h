#ifndef KU_LIBCOMMON_TIMER_H
#define KU_LIBCOMMON_TIMER_H

#pragma once

#include "common.h"
#include "loop.h"
#include "noncopyable.h"

#include <functional>
#include <stdint.h>

KU_NS_LEFT
namespace common
{

class timer : public noncopyable {
public:
	timer(std::function<void()> &&cb, loop *p = nullptr);
	~timer();

	bool is_started() const;

	bool start(uint64_t timeout, uint64_t repeat = 0);
	bool stop();

private:
	static void timer_callback(void *handle);

private:
	std::function<void()> callback_;
	void *timer_;
	bool started_;
};

//
// inline
//

inline bool timer::is_started() const
{
	return started_;
}

} // namespace common
KU_NS_RIGHT

#endif
