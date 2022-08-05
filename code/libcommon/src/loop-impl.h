
#include <memory>
#include <mutex>
#include <list>

#include <stdlib.h>

#include "libcommon/loop.h"
#include "uv.h"

KU_NS_LEFT
namespace common
{

class _loop_impl {
	friend class loop;

public:
	_loop_impl(bool use_default);
	~_loop_impl();

	uv_loop_t *get_loop() const;

	int run_loop(bool once = false, bool nowait = false);

	void stop_loop();

public:
	static void on_closed_and_free_cb(uv_handle_t *handle);

protected:
	uv_loop_t *loop_;
	void *loop_thread_;
	uv_async_t *work_async_;
	std::mutex work_mutex_;
	std::list<std::function<void()> > work_list_;
};

//
// inline
//

inline uv_loop_t *_loop_impl::get_loop() const
{
	return loop_;
}

} // namespace common
KU_NS_RIGHT
