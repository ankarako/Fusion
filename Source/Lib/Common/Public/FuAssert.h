#ifndef __COMMON_PUBLIC_FUASSERT_H__
#define __COMMON_PUBLIC_FUASSERT_H__

#include <cassert>
/// Debug assert
///	asserts only when the build configuration is debug
#if defined(_DEBUG) || defined(NDEBUG)
	#define DebugAssert(predicate)			assert(predicate)
	#define DebugAssertMsg(predicate, msg)	assert(predicate, msg)
#else
	#define DebugAssert(predicate)
	#define DebugAssertMsg(predicate, msg)
#endif	///	!_DEBUG
/// HardAsserts always assert
#define HardAssert(predicate)			assert(predicate)
#define HardAssertMsg(predicate, msg)	assert(predicate, msg)

#endif	///	!__COMMON_PUBLIC_FUASSERT_H__