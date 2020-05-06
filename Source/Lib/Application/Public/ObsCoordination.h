#ifndef	__APPLICATION_PUBLIC_WTF_OBSCOORDINATION_H__
#define __APPLICATION_PUBLIC_WTF_OBSCOORDINATION_H__

#include <rxcpp/rx.hpp>

namespace fu {
namespace app {
///	\class ObsCoordination
///	\brief an observable coordination interface
class ObsCoordination
{
public:
	ObsCoordination() { }
	///	\typedef UICoordinationType
	///	\brief UICoordination callables should be scheduled on one thread only
	using UICoordinationType = typename rxcpp::observe_on_one_worker;
	///	\typedef ModelCoordinationType
	///	\brief
	using ModelCoordinationType = typename rxcpp::identity_one_worker;
	///	\brief coordinate callable on model thread
	virtual ModelCoordinationType ModelCoordination() const = 0;
	///	\brief coordinate callable on ui thread
	virtual UICoordinationType UICoordination() const = 0;
};	///	!class ObsCoordination
}	///	!namespace app
}	///	!namespace fu
#endif	///	!__APPLICATION_PUBLIC_WTF_OBSCOORDINATION_H__