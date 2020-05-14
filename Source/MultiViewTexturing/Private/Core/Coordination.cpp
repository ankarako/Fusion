#include <Core/Coordination.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief Coordination;s implementation
struct Coordination::Impl
{
	typedef rxcpp::observe_on_one_worker ObserveOnCoordinationType;
	ObserveOnCoordinationType	m_CoreModelCoordination;
	ModelCoordinationType		m_ModelCoordination;
	UICoordinationType			m_UICoordination;
	/// Construction
	Impl(ObserveOnCoordinationType coreModel, UICoordinationType ui)
		: m_CoreModelCoordination(coreModel)
		, m_ModelCoordination(rxcpp::identity_same_worker(m_CoreModelCoordination.create_coordinator().get_worker()))
		, m_UICoordination(ui)
	{ }
};	///	!struct Impl
/// Construction
Coordination::Coordination(rxcpp::schedulers::run_loop& rl)
	: m_Impl(spimpl::make_unique_impl<Impl>(rxcpp::observe_on_event_loop(), rxcpp::observe_on_run_loop(rl)))
{ }

Coordination::ModelCoordinationType Coordination::ModelCoordination() const
{
	return m_Impl->m_ModelCoordination;
}

Coordination::UICoordinationType Coordination::UICoordination() const
{
	return m_Impl->m_UICoordination;
}
}	///	!namespace mvt
}	///	!namespace fu