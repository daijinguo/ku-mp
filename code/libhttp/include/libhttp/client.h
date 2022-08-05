#ifndef KU_LIBHTTP_CLIENT_H
#define KU_LIBHTTP_CLIENT_H

#pragma once

#include "libcommon/common.h"
#include "libcommon/loop.h"

using namespace KU_NS::common;

KU_NS_LEFT
namespace http
{

class client : public loop {
public:
	client(bool use_default);
	~client() override;
};

} // namespace http
KU_NS_RIGHT

#endif // KU_LIBHTTP_CLIENT_H
