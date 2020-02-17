#include <Presenters/PlayerControllerPresenter.h>
#include <Models/PlayerModel.h>
#include <Views/PlayerControllerView.h>
#include <WidgetRepo.h>

namespace fusion {
///	\struct Impl
///	\brief PlayerControler Implementation
struct PlayerControllerPresenter::Impl
{
	model_ptr_t	m_Model;
	view_ptr_t	m_View;
	wrepo_ptr_t m_WidgetRepo;
	///	Construction
	Impl(model_ptr_t model, view_ptr_t view, wrepo_ptr_t wrepo)
		: m_Model(model), m_View(view), m_WidgetRepo(wrepo)
	{ }
};	///	!struct Impl
///	Construction
PlayerControllerPresenter::PlayerControllerPresenter(model_ptr_t model, view_ptr_t view, wrepo_ptr_t wrepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, view, wrepo))
{ }
///	\brief Initialize the presenter
void PlayerControllerPresenter::Init()
{
	m_Impl->m_View->OnActivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_WidgetRepo->RegisterWidget(m_Impl->m_View);
	});

	m_Impl->m_View->OnDeactivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_WidgetRepo->UnregisterWidget(m_Impl->m_View);
	});

	m_Impl->m_View->Activate();
}
}	///	!namespace fusion