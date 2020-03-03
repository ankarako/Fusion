#include <Presenters/PlayerControllerPresenter.h>
#include <Views/PlayerControllerView.h>
#include <Views/FileExplorerView.h>
#include <Models/PlayerModel.h>
#include <Core/SimpleTask.h>
#include <WidgetRepo.h>
#include <plog/Log.h>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief PlayerControler Implementation
struct PlayerControllerPresenter::Impl
{
	model_ptr_t		m_Model;
	view_ptr_t		m_View;
	fexp_view_ptr_t	m_FexpView;
	wrepo_ptr_t		m_WidgetRepo;
	SimpleTask		m_ActivateWidgetTask;
	SimpleTask		m_DeactivateWidgetTask;
	SimpleTask		m_LoadFileTask;
	
	
	///	Construction
	Impl(model_ptr_t model, view_ptr_t view, fexp_view_ptr_t fexp_view, wrepo_ptr_t wrepo)
		: m_Model(model)
		, m_View(view)
		, m_FexpView(fexp_view)
		, m_WidgetRepo(wrepo)
	{ }
};	///	!struct Impl
///	Construction
PlayerControllerPresenter::PlayerControllerPresenter(
	model_ptr_t model, 
	view_ptr_t view, 
	fexp_view_ptr_t fexp_view, 
	wrepo_ptr_t wrepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, view, fexp_view, wrepo))
{ }
///	\brief Initialize the presenter
void PlayerControllerPresenter::Init()
{
	/// view activated event task
	m_Impl->m_View->OnActivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_WidgetRepo->RegisterWidget(m_Impl->m_View);
	});
	/// view deactivated event task
	m_Impl->m_View->OnDeactivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_WidgetRepo->UnregisterWidget(m_Impl->m_View);
	});
	///	file explorer view open video event task
	m_Impl->m_FexpView->OpenVideoFileFlowOut().subscribe(
		[this](auto filepath)
	{
		m_Impl->m_Model->LoadFile(filepath);
		m_Impl->m_View->Activate();
	});
	///	seek back event task
	m_Impl->m_View->OnSeekBackwardButtonClicked().subscribe(
		[this](auto _)
	{
		
	});
	/// play event task
	m_Impl->m_View->OnPlayButtonClicked().observe_on(rxcpp::observe_on_new_thread()).subscribe(
		[this](auto _)
	{
		m_Impl->m_Model->Start();
	});
	/// pause event task
	m_Impl->m_View->OnPauseButtonClicked().subscribe(
		[this](auto _) 
	{
		m_Impl->m_Model->Pause();
	});
	/// stop event task
	m_Impl->m_View->OnStopButtonClicked().subscribe(
		[this](auto _)
	{
		m_Impl->m_Model->Stop();
	});
	/// seek forward event task
	m_Impl->m_View->OnSeekForwardButtonClicked().subscribe(
		[this](auto _)
	{

	});

	m_Impl->m_Model->FrameCountFlowOut().subscribe(
		[this](size_t count) 
	{
		m_Impl->m_View->SetMaxSliderValue(count);
	});

	m_Impl->m_Model->CurrentFrameIdFlowOut().subscribe(
		[this](size_t pos) 
	{
		m_Impl->m_View->SetSliderValue(pos);
	});
	m_Impl->m_View->Deactivate();
}
///	\brief destroy the presenter
///	destroys the decoding context
void PlayerControllerPresenter::Destroy()
{
	
}
}	///	!namespace fusion
}	///	!namespace fu