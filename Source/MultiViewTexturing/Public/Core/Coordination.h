#ifndef __MVT_PUBLIC_CORE_COORDINATION_H__
#define __MVT_PUBLIC_CORE_COORDINATION_H__

#include <NoCopyAssign.h>
#include <ObsCoordination.h>
#include <rxcpp/rx.hpp>
#include <memory>
#include <spimpl.h>

namespace fu {
namespace mvt {
///	\class Coordination
///	\brief MVT's scheduler
class Coordination final : app::ObsCoordination
{
public:
	NoCopyAssign(Coordination);

	explicit Coordination(rxcpp::schedulers::run_loop& rl);
	Coordination(Coordination&& other) = default;
	~Coordination() = default;
	Coordination& operator=(Coordination&&) = delete;
	///
	virtual ModelCoordinationType ModelCoordination() const override;
	virtual UICoordinationType UICoordination() const override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class Coordination
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_CORE_COORDINATION_H__