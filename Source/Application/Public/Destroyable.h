#ifndef __APPLICATION_PUBLIC_DESTROYABLE_H__
#define __APPLICATION_PUBLIC_DESTROYABLE_H__

namespace app {
///	\class Destroyable
///	\brief a simple interfaces that provides destruction utility
class Destroyable
{
public:
	virtual void Destroy() = 0;
	virtual ~Destroyable() = default;
};	///	!namespace Destroyable
}	///	!namespace app
#endif	///	!__APPLICATION_PUBLIC_DESTROYABLE_H__