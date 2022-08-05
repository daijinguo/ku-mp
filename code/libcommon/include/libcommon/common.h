#ifndef KU_LIBCOMMON_COMMON_H
#define KU_LIBCOMMON_COMMON_H

#pragma once

// clang-format off
#ifndef KU_OME
#	define KU_OME oem
#endif

#ifndef KU_NS
#	define KU_NS KU_OME::ku
#endif

#ifndef KU_NS_LEFT
#	define KU_NS_LEFT namespace KU_OME { namespace ku {
#endif

#ifndef KU_NS_RIGHT
#	define KU_NS_RIGHT                 }              }
#endif
// clang-format on

#endif //KU_LIBCOMMON_COMMON_H
