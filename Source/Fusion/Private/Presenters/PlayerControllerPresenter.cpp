#include <Presenters/PlayerControllerPresenter.h>
#include <Models/PlayerModel.h>
#include <Views/PlayerControllerView.h>
#include <Views/FileExplorerView.h>
#include <WidgetRepo.h>
#include <plog/Log.h>

namespace fusion {
///	\struct Impl
///	\brief PlayerControler Implementation
struct PlayerControllerPresenter::Impl
{
	model_ptr_t		m_Model;
	view_ptr_t		m_View;
	fexp_view_ptr_t	m_FexpView;
	wrepo_ptr_t		m_WidgetRepo;
	///	Construction
	Impl(model_ptr_t model, view_ptr_t view, fexp_view_ptr_t fexp_view, wrepo_ptr_t wrepo)
		: m_Model(model), m_View(view), m_FexpView(fexp_view), m_WidgetRepo(wrepo)
	{ }
};	///	!struct Impl
///	Construction
PlayerControllerPresenter::PlayerControllerPresenter(model_ptr_t model, view_ptr_t view, fexp_view_ptr_t  fexp_view, wrepo_ptr_t wrepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, view, fexp_view, wrepo))
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

	m_Impl->m_FexpView->OpenVideoFileFlowOut().subscribe(
		[this](auto filepath) 
	{
		m_Impl->m_Model->LoadFile(filepath);
		m_Impl->m_View->Activate();
	}, [this](std::exception_ptr ex_ptr) 
	{
		try {
			if (ex_ptr)
			{
				std::rethrow_exception(ex_ptr);
			}
		}
		catch (std::exception& ex)
		{
			LOG_ERROR << ex.what();
			m_Impl->m_Model->Destroy();
		}
	});

	m_Impl->m_View->OnSeekBackwardButtonClicked().subscribe(
		[this](auto _) 
	{
	
	});

	m_Impl->m_View->OnPlayButtonClicked().subscribe(
		[this](auto _) 
	{
		m_Impl->m_Model->Play();
	});

	m_Impl->m_View->OnStopButtonClicked().subscribe(
		[this](auto _) 
	{
		m_Impl->m_Model->Stop();
	});

	m_Impl->m_View->OnSeekForwardButtonClicked().subscribe(
		[this](auto _) 
	{
		
	});

	m_Impl->m_View->Deactivate();
}
}	///	!namespace fusion