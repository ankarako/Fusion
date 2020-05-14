#include <Core/UIRxDispatcher.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief UIRxDispatcher implementation
struct UIRxDispatcher::Impl
{
	rxcpp::schedulers::run_loop& m_RunLoop;
	/// Construction
	Impl(rxcpp::schedulers::run_loop& rl)
		: m_RunLoop(rl)
	{ }
};	///	!struct Impl
/// Construction
UIRxDispatcher::UIRxDispatcher(rxcpp::schedulers::run_loop& rl)
	: m_Impl(spimpl::make_unique_impl<Impl>(rl))
{ }
///	\brief it will run pending tasks
/// see example: https://github.com/ReactiveX/RxCpp/issues/151
void UIRxDispatcher::Update()
{
	auto& rl = m_Impl->m_RunLoop;
	while (!rl.empty() && rl.peek().when < rl.now())
		rl.dispatch();
}
}	///	!namespace mvt
}	///	!namespace fu