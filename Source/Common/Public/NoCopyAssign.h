#ifndef __COMMON_PUBLIC_NOCOPYASSIGN_H__
#define __COMMON_PUBLIC_NOCOPYASSIGN_H__

#define NoCopyAssign(ObjectName)						\
	ObjectName(const ObjectName&) = delete;				\
	ObjectName& operator=(const ObjectName&) = delete;

#define NoCopyAssignMove(ObjectName)					\
	ObjectName(const ObjectName&) = delete;				\
	ObjectName(ObjectName&&) = delete;					\
	ObjectName& operator=(const ObjectName&) = delete;	\
	ObjectName& operator=(ObjectName&&) = delete;		

#endif	///	!__COMMON_PUBLIC_NOCOPYASSIGN_H__