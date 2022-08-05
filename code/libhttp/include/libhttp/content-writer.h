#ifndef KU_LIBHTTP_CONTENT_WRITER_H
#define KU_LIBHTTP_CONTENT_WRITER_H

#pragma once

#include "libcommon/common.h"
#include "libcommon/loop.h"
#include "common.h"

#include <list>
#include <memory>

using namespace KU_NS::common;

KU_NS_LEFT
namespace http
{

struct write_req;

class content_writer {
public:
	content_writer(loop *);
	virtual ~content_writer();

	int start_write(std::shared_ptr<std::string> headers, content_provider provider);

protected:
	virtual void on_write_end(int error_code) = 0;

	bool is_write_done();
	void set_write_done();

	void prepare_next();

	int write_content(std::shared_ptr<write_req> req);
	int write_next();

protected:
	int64_t content_written_;
	int64_t content_to_write_;
	void *socket_;
	loop *loop_;

private:
	static void on_written_cb(void *req_t, int status);

private:
	content_sink content_sink_;
	content_provider content_provider_;

	bool headers_written_{ false };
	int last_socket_error_{ 0 };
	std::shared_ptr<write_req> writing_req_;
	std::list<std::shared_ptr<write_req> > req_list_;
};

//
// inline functions
//

inline bool content_writer::is_write_done()
{
	return content_written_ >= content_to_write_;
}

inline void content_writer::set_write_done()
{
	content_to_write_ = 0;
}

} // namespace http
KU_NS_RIGHT

#endif