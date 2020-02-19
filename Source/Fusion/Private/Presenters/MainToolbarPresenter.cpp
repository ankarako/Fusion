#include <Presenters/MainToolbarPresenter.h>
#include <WidgetRepo.h>
#include <Models/PlayerModel.h>
#include <Views/MainToolbarView.h>
#include <Views/FileExplorerView.h>

namespace fusion {
///	\struct Impl
///	\brief MainToolbarPresenter implementation
struct MainToolbarPresenter::Impl
{
	view_ptr_t			m_View;
	fexp_view_ptr_t		m_FexpView;
	wrepo_ptr_t			m_Wrepo;
	player_model_ptr_t	m_PlayerModel;

	Impl(player_model_ptr_t player_model, view_ptr_t view, fexp_view_ptr_t fexp_view, wrepo_ptr_t wrepo)
		: m_View(view), m_FexpView(fexp_view), m_Wrepo(wrepo), m_PlayerModel(player_model)
	{ }
};	///	!struct Impl
///	Construction
MainToolbarPresenter::MainToolbarPresenter(player_model_ptr_t player_model, view_ptr_t view, fexp_view_ptr_t fexp_view, wrepo_ptr_t wrepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(player_model, view, fexp_view, wrepo))
{ }
///	\brief Initialization
void MainToolbarPresenter::Init()
{
	///	Widget activation
	m_Impl->m_View->OnActivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_View);
	});
	/// Widget deactivation
	m_Impl->m_View->OnDeactivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_View);
	});

	m_Impl->m_View->OnFileMenu_OpenProjectClicked().subscribe(
		[this](auto _) 
	{
		m_Impl->m_FexpView->SetMode(FileExplorerMode::OpenProject);
		m_Impl->m_FexpView->Activate();
	});

	m_Impl->m_View->OnFileMenu_SaveProjectAsClicked().subscribe(
		[this](auto _) 
	{
		m_Impl->m_FexpView->SetMode(FileExplorerMode::SaveProject);
		m_Impl->m_FexpView->Activate();
	});

	m_Impl->m_View->OnFileMenu_OpenVideoFileClicked().subscribe(
		[this](auto _) 
	{
		m_Impl->m_FexpView->SetMode(FileExplorerMode::OpenVideoFile);
		m_Impl->m_FexpView->Activate();
	});

	m_Impl->m_View->Activate();
}
}	///	!namespace fusion