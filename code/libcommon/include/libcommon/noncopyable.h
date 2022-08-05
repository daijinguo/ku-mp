#ifndef KU_LIBCOMMON_NONCOPYABLE_H
#define KU_LIBCOMMON_NONCOPYABLE_H

#pragma once

class noncopyable {
protected:
	noncopyable() = default;
	~noncopyable() = default;

private:
	noncopyable(const noncopyable &) = delete;
	noncopyable(noncopyable &&) = delete;
	noncopyable &operator=(const noncopyable &) = delete;
	noncopyable &operator=(noncopyable &&) = delete;
};

#endif
