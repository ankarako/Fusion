#ifndef __COMMON_PUBLIC_DEBUGMSG_H__
#define __COMMON_PUBLIC_DEBUGMSG_H__

#include <plog/Log.h>

#if defined(_DEBUG)
	#define DebugMsg(msg)	LOG_DEBUG << msg;
#else
	#define DebugMsg(msg)
#endif

namespace fu { }
#endif	///	!__COMMON_PUBLIC_DEBUGMSG_H__