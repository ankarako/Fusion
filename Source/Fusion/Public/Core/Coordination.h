#ifndef __FUSION_PUBLIC_CORE_COORDINATION_H__
#define __FUSION_PUBLIC_CORE_COORDINATION_H__

#include <NoCopyAssign.h>
#include <ObsCoordination.h>
#include <rxcpp/rx.hpp>
#include <memory>
#include <spimpl.h>

namespace fu {
namespace fusion {
///	\class Coordination
///	\brief fusion's scheduler
class Coordination final : app::ObsCoordination
{
public:
	NoCopyAssign(Coordination);
	///	\typedef UICoordinationType
	///	\brief UICoordination callables should be scheduled on one thread only
	typedef rxcpp::observe_on_one_worker	UICoordinationType;
	///	\typedef ModelCoordinationType
	///	\brief
	typedef rxcpp::identity_one_worker		ModelCoordinationType;
	/// Construction
	explicit Coordination(rxcpp::schedulers::run_loop& rl);
	/// 
	Coordination(Coordination&& other) = default;
	/// Destruction
	~Coordination() = default;
	Coordination& operator=(Coordination&&) = delete;
	///
	virtual ModelCoordinationType ModelCoordination() const override;
	///
	virtual UICoordinationType		UICoordination() const override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class Coodination
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_CORE_COORDINATION_H__