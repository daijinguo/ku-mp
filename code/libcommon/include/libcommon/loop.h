#ifndef KU_LIBCOMMON_LOOP_H
#define KU_LIBCOMMON_LOOP_H

#pragma once

#include "common.h"
#include <functional>
#include <stdint.h>

KU_NS_LEFT
namespace common
{

class _loop_impl;
class loop {
protected:
	_loop_impl *impl_;

	void on_async();
	static void on_async_cb(void *handle);

public:
	loop(bool use_default = false);
	virtual ~loop();

	int async(std::function<void()> &&work);

	bool queue_work(std::function<intptr_t()> &&work, std::function<void(intptr_t)> &&done = nullptr);

	void *get_loop() const;

	int run_loop(bool once = false, bool nowait = false);

	void stop_loop();
};

} // namespace common
KU_NS_RIGHT

#endif //KU_LIBCOMMON_LOOP_H
