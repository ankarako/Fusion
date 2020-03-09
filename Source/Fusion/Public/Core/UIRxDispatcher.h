#ifndef __FUSION_PUBLIC_CORE_UIRXDISPATCHER_H__
#define __FUSION_PUBLIC_CORE_UIRXDISPATCHER_H__

#include <Updateable.h>
#include <rxcpp/rx.hpp>
#include <spimpl.h>

namespace fu {
namespace fusion {
///	\class UIRxDispathcer
///	\brief it will run pending handlers on UI tasks
class UIRxDispatcher : public app::Updateable
{
public:
	/// Construction
	explicit UIRxDispatcher(rxcpp::schedulers::run_loop& rl);
	///	 Update
	void Update() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class UIRxDispatcher
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_CORE_UIRXDISPATCHER_H__