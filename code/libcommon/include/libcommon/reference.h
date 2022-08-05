#ifndef KU_LIBCOMMON_REFERENCE_H
#define KU_LIBCOMMON_REFERENCE_H

#pragma once

#include <assert.h>

// clang-format off
#define REFERENCE_COUNT(T) \
	private: \
		int ref_count_ = 1; \
	public: \
		inline T *aquire() \
		{ \
			ref_count_++; \
			return this; \
		} \
		inline void release() \
		{ \
			assert(ref_count_ > 0); \
			if (--ref_count_ == 0) \
				delete this; \
		}
// clang-format on

#endif //KU_LIBCOMMON_REFERENCE_H
