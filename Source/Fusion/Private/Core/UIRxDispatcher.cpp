#include <Core/UIRxDispatcher.h>

namespace fu {
namespace fusion {
///	`\struct Impl
///	\brief UIRxDispathcer implementation
struct UIRxDispatcher::Impl
{
	rxcpp::schedulers::run_loop& m_RunLoop;
	Impl(rxcpp::schedulers::run_loop& rl)
		: m_RunLoop(rl)
	{ }
};
/// Construction
UIRxDispatcher::UIRxDispatcher(rxcpp::schedulers::run_loop& rl)
	: m_Impl(spimpl::make_unique_impl<Impl>(rl))
{ }
/// Update
///	\brief it will run pending tasks
/// see example: https://github.com/ReactiveX/RxCpp/issues/151
void UIRxDispatcher::Update()
{
	auto& rl = m_Impl->m_RunLoop;
	while (!rl.empty() && rl.peek().when < rl.now())
		rl.dispatch();
}
}	///	!namespace fusion
}	///	!namespace fu