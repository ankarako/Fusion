#include <Presenters/MainToolbarPresenter.h>
#include <Views/MainToolbarView.h>
#include <WidgetRepo.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief MainToolbarPresenter Implementation
struct MainToolbarPresenter::Impl
{
	view_ptr_t 	m_View;
	wrepo_ptr_t m_Wrepo;
	/// Construction
	Impl(view_ptr_t view, wrepo_ptr_t wrepo)
		: m_View(view)
		, m_Wrepo(wrepo)
	{ }
};	///	!struct Impl
/// Construction
MainToolbarPresenter::MainToolbarPresenter(view_ptr_t view, wrepo_ptr_t wrepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, wrepo))
{ }
///	\brief Initialize the presenter
void MainToolbarPresenter::Init()
{
	///=====================================
	/// register the view to the widget repo
	///=====================================
	m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_View);
	m_Impl->m_View->Activate();
}
}	///	!namespace mvt
}	///	!namespace fu