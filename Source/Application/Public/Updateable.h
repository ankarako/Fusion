#ifndef	__APPLICATION_PUBLIC_UPDATEABLE_H__
#define __APPLICATION_PUBLIC_UPDATEABLE_H__

namespace app {
///	\class Updateable
///	\brief a simple interface that provides update logic
class Updateable
{
public:
	virtual void Update() = 0;
	virtual ~Updateable() = default;
};	///	!class Updateable
}	///	!namespace app
#endif	///	!__APPLICATION_PUBLIC_UPDATEABLE_H__