#include <Presenters/ViewportPresenter.h>
#include <Views/ViewportView.h>
#include <WidgetRepo.h>
#include <Core/Coordination.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief ViewportPresenter initialization
struct ViewportPresenter::Impl
{
	view_ptr_t 	m_View;
	wrepo_ptr_t	m_Wrepo;
	coord_ptr_t m_Coord;
	/// Construction
	Impl(view_ptr_t view, wrepo_ptr_t wrepo, coord_ptr_t coord)
		: m_View(view)
		, m_Wrepo(wrepo)
		, m_Coord(coord)
	{ }
};	///	!struct Impl
/// Construction
ViewportPresenter::ViewportPresenter(view_ptr_t view, wrepo_ptr_t wrepo, coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, wrepo, coord))
{ }
/// \brief presenter initialization
void ViewportPresenter::Init()
{
	///===============================
	/// Window activation/deactivation
	///===============================
	m_Impl->m_View->OnActivated()
		.subscribe([this](auto _)
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_View);
	});
	m_Impl->m_View->OnDeactivated()
		.subscribe([this](auto _)
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_View);
	});
	/// Activate the view
	m_Impl->m_View->Activate();
}
}	///	!namespace mvt
}	///	!namespace fu