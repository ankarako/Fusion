#ifndef __MVT_PUBLIC_CORE_UIRXDISPATCHER_H__
#define __MVT_PUBLIC_CORE_UIRXDISPATCHER_H__

#include <Updateable.h>
#include <rxcpp/rx.hpp>
#include <spimpl.h>

namespace fu {
namespace mvt {
///	\class UIRxDispatcher
///	\brief it will run pending handlers on UI tasks
class UIRxDispatcher : public app::Updateable
{
public:
	///	Construction
	explicit UIRxDispatcher(rxcpp::schedulers::run_loop& rl);
	/// Upodate
	void Update() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class UIRxDispatcher
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_CORE_UIRXDISPATCHER_H__