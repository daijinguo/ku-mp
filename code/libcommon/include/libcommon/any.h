#ifndef KU_LIBCOMMON_ANY_H
#define KU_LIBCOMMON_ANY_H

#pragma once

#include "common.h"
#include <memory>

KU_NS_LEFT
namespace common
{

class any : public std::enable_shared_from_this<any> {
public:
	using ptr = std::shared_ptr<any>;

	any();
	virtual ~any();

	template <typename T> static typename T::ptr shared_from_this(T *p_this);
};

template <typename T> typename T::ptr any::shared_from_this(T *p_this)
{
	return std::static_pointer_cast<T>(p_this->shared_from_this());
}

} // namespace common
KU_NS_RIGHT

#endif
