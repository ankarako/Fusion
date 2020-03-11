#include <Presenters/PlayerControllerPresenter.h>
#include <Views/PlayerControllerView.h>
#include <Views/FileExplorerView.h>
#include <Models/PlayerModel.h>
#include <Core/Coordination.h>
#include <WidgetRepo.h>
#include <plog/Log.h>
#include <rxcpp/rx-scheduler.hpp>

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
	coord_ptr_t		m_Coord;
	
	///	Construction
	Impl(model_ptr_t model, view_ptr_t view, fexp_view_ptr_t fexp_view, wrepo_ptr_t wrepo, coord_ptr_t coord)
		: m_Model(model)
		, m_View(view)
		, m_FexpView(fexp_view)
		, m_WidgetRepo(wrepo)
		, m_Coord(coord)
	{ }
};	///	!struct Impl
///	Construction
PlayerControllerPresenter::PlayerControllerPresenter(
	model_ptr_t model, 
	view_ptr_t view, 
	fexp_view_ptr_t fexp_view, 
	wrepo_ptr_t wrepo,
	coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, view, fexp_view, wrepo, coord))
{ }
///	\brief Initialize the presenter
void PlayerControllerPresenter::Init()
{
	///==========================
	/// view activated event task
	///==========================
	m_Impl->m_View->OnActivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_WidgetRepo->RegisterWidget(m_Impl->m_View);
	});
	///============================
	/// view deactivated event task
	///============================
	m_Impl->m_View->OnDeactivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_WidgetRepo->UnregisterWidget(m_Impl->m_View);
	});
	///==========================================
	///	file explorer view open video event task
	///===========================================
	m_Impl->m_FexpView->OpenVideoFileFlowOut().subscribe(
		[this](auto filepath)
	{
		m_Impl->m_Model->LoadFile(filepath);
		m_Impl->m_View->Activate();
	});
	///======================
	///	seek back event task
	///======================
	m_Impl->m_View->OnSeekBackwardButtonClicked().subscribe(
		[this](auto _)
	{
		m_Impl->m_Model->SeekBackward();
	});
	///================
	/// play event task
	///================
	m_Impl->m_View->OnPlayButtonClicked().observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](auto _)
	{
		m_Impl->m_Model->Start();
	}, [this](std::exception_ptr& ex) 
	{
		if (ex)
		{
			try
			{
				std::rethrow_exception(ex);
			}
			catch (std::exception & ex)
			{
				LOG_ERROR << "wtf." << ex.what();
			}
		}
			
	});
	///==================
	/// pause event task
	///==================
	m_Impl->m_View->OnPauseButtonClicked().subscribe(
		[this](auto _) 
	{
		m_Impl->m_Model->Pause();
	});
	///=================
	/// stop event task
	///=================
	m_Impl->m_View->OnStopButtonClicked().subscribe(
		[this](auto _)
	{
		m_Impl->m_Model->Stop();
	});
	///========================
	/// seek forward event task
	///========================
	m_Impl->m_View->OnSeekForwardButtonClicked().subscribe(
		[this](auto _)
	{
		m_Impl->m_Model->SeekForward();
	});

	///===================================
	/// Model -> FrameCount Flow out task
	///===================================
	m_Impl->m_Model->FrameCountFlowOut().subscribe(
		[this](size_t count) 
	{
		m_Impl->m_View->SetMaxSliderValue(count);
	});
	///================================
	///	Model -> Frame id flow out task
	///================================
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