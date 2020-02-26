#include <Presenters/PlayerViewportPresenter.h>
#include <Views/PlayerViewportView.h>
#include <WidgetRepo.h>

namespace fu {
namespace fusion {
struct PlayerViewportPresenter::Impl
{
	model_ptr_t	m_Model;
	view_ptr_t	m_View;
	wrepo_ptr_t m_Wrepo;

	Impl(model_ptr_t model, view_ptr_t view, wrepo_ptr_t wrepo)
		: m_Model(model), m_View(view), m_Wrepo(wrepo)
	{ }
};
/// Construction
PlayerViewportPresenter::PlayerViewportPresenter(model_ptr_t model, view_ptr_t view, wrepo_ptr_t wrepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, view, wrepo))
{ }

void PlayerViewportPresenter::Init()
{
	m_Impl->m_View->OnActivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_View);
	});

	m_Impl->m_View->OnDeactivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_View);
	});

	m_Impl->m_View->OnViewportWidthChanged().subscribe(
		[this](float newWidth)
	{
		
	});

	m_Impl->m_View->Activate();
}
}	///	!namespace fusion
}	///	!namespace fu