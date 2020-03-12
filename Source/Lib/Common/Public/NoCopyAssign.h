#ifndef __COMMON_PUBLIC_NOCOPYASSIGN_H__
#define __COMMON_PUBLIC_NOCOPYASSIGN_H__

///	\brief deny copy constructor and copy-assignment on a class
///	\param	className	the name of the class
#define NoCopyAssign(className)								\
	className(const className& inClass) = delete;			\
	className& operator=(const className& inClass) = delete;
///	\brief deny copy/move constructor and copy/move-assignment on a class
///	\param	className	the name of the class
#define NoCopyAssignMove(className)								\
	className(const className& inClass) = delete;				\
	className(className&& inClass) = delete;					\
	className& operator=(const className& inClass) = delete;	\
	className& operator=(className&& inClass) = delete;		

#endif	///	!__COMMON_PUBLIC_NOCOPYASSIGN_H__