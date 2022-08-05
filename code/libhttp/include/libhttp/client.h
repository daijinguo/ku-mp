#ifndef KU_LIBHTTP_CLIENT_H
#define KU_LIBHTTP_CLIENT_H

#pragma once

#include "libcommon/common.h"
#include "libcommon/loop.h"
#include "common.h"

#include <stdlib.h>
#include <memory>
#include <functional>
#include <unordered_map>

using namespace KU_NS::common;

KU_NS_LEFT
namespace http
{

// clang-format off
using on_response     = std::function<bool(const response &res)>;
using on_content      = std::function<bool(const char *data, size_t size, bool end)>;
using on_content_body = std::function<void(const std::string &body, int error)>;
using on_redirect     = std::function<bool(std::string &url)>;
using on_error        = std::function<void(int code)>;
// clang-format on

class _socket_checker;
class client : public loop {
public:
	client(bool use_default);
	~client() override;

	// clang-format off
	int fetch(const request &req,
			  on_response  &&on_resp,
			  on_content   &&on_content,
			  on_redirect  &&on_redirect = nullptr,
			  on_error     &&on_error = nullptr);

	int fetch(const request    &request,
	          on_content_body &&on_body,
              on_response     &&on_response = nullptr,
		      on_redirect     &&on_redirect = [](std::string &url) { return true; });
	// clang-format on

private:
	std::shared_ptr<class buffer_pool> buffer_pool_;
	std::shared_ptr<std::unordered_multimap<std::string, _socket_checker *> > socket_cache_;
};

} // namespace http
KU_NS_RIGHT

#endif // KU_LIBHTTP_CLIENT_H
