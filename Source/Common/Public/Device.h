#ifndef	__COMMON_PUBLIC_DEVIDE_H__
#define __COMMON_PUBLIC_DEVIDE_H__

namespace fu {
///	\enum Device
///	\brief	device enumeration can be either CPU or GPU
enum class Device
{
	Unknown = 0,
	CPU,
	GPU
};	///	!enum Device
}	///	!namespace fu
#endif	///	!__COMMON_PUBLIC_DEVIDE_H__