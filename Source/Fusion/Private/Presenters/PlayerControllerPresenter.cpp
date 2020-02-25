#include <Presenters/PlayerControllerPresenter.h>
#include <Views/PlayerControllerView.h>
#include <Views/FileExplorerView.h>
#include <Core/SimpleTask.h>
#include <Rational.h>
#include <DecodingContext.h>
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
	executor_ptr_t	m_Executor;
	SimpleTask		m_ActivateWidgetTask;
	SimpleTask		m_DeactivateWidgetTask;
	SimpleTask		m_LoadFileTask;
	tf::Taskflow	m_PlaybackTaskFlow;
	tf::Task		m_PlayTask = m_PlaybackTaskFlow.placeholder();
	tf::Task		m_WaitForTimer = m_PlaybackTaskFlow.placeholder();
	Rational		m_FrameRate;
	
	///	Construction
	Impl(model_ptr_t model, view_ptr_t view, fexp_view_ptr_t fexp_view, wrepo_ptr_t wrepo, executor_ptr_t exec)
		: m_Model(model)
		, m_View(view)
		, m_FexpView(fexp_view)
		, m_WidgetRepo(wrepo)
		, m_Executor(exec)
	{ }
};	///	!struct Impl
///	Construction
PlayerControllerPresenter::PlayerControllerPresenter(
	model_ptr_t model, 
	view_ptr_t view, 
	fexp_view_ptr_t fexp_view, 
	wrepo_ptr_t wrepo, 
	executor_ptr_t exec)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, view, fexp_view, wrepo, exec))
{ }
///	\brief Initialize the presenter
void PlayerControllerPresenter::Init()
{
	/// Task creation
	m_Impl->m_ActivateWidgetTask.Task = m_Impl->m_ActivateWidgetTask.Flow.emplace(
		[this]()
	{
		m_Impl->m_WidgetRepo->RegisterWidget(m_Impl->m_View);
	}).name("ActivateWidgetTask");
	/// 
	m_Impl->m_DeactivateWidgetTask.Task = m_Impl->m_DeactivateWidgetTask.Flow.emplace(
		[this]()
	{
		m_Impl->m_WidgetRepo->RegisterWidget(m_Impl->m_View);
	}
	).name("DeactivateWidget");
	///
	m_Impl->m_PlayTask = m_Impl->m_PlaybackTaskFlow.emplace(
		[this]()
	{
		if (!m_Impl->m_Model->DecodeCurrentFrame())
		{
			m_Impl->m_Model->Terminate();
			m_Impl->m_Model->Initialize();
			m_Impl->m_View->Deactivate();
		}
	});

	m_Impl->m_WaitForTimer = m_Impl->m_PlaybackTaskFlow.emplace(
		[this]()
	{
		
	});

	m_Impl->m_PlayTask.precede(m_Impl->m_WaitForTimer);

	/// view activated event task
	m_Impl->m_View->OnActivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_Executor->run(m_Impl->m_ActivateWidgetTask.Flow).wait();
	});
	/// view deactivated event task
	m_Impl->m_View->OnDeactivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_Executor->run(m_Impl->m_DeactivateWidgetTask.Flow).wait();
	});
	///	file explorer view open video event task
	m_Impl->m_FexpView->OpenVideoFileFlowOut().subscribe(
		[this](auto filepath)
	{
		m_Impl->m_LoadFileTask.Task = m_Impl->m_LoadFileTask.Flow.emplace(
			[this, filepath]()
		{
			if (m_Impl->m_Model->IsFileLoaded())
			{
				m_Impl->m_Model->Terminate();
				m_Impl->m_Model->Initialize();
			}
			if (m_Impl->m_Model->LoadFile(filepath))
			{
				m_Impl->m_View->Activate();
			}
			else
			{
				/// means that the file could not be loaded
				m_Impl->m_Model->Terminate();
				m_Impl->m_Model->Initialize();
			}
		}).name("LoadFile");
		m_Impl->m_Executor->run(m_Impl->m_LoadFileTask.Flow).wait();
		m_Impl->m_LoadFileTask.Flow.clear();
	});
	///	seek back event task
	m_Impl->m_View->OnSeekBackwardButtonClicked().subscribe(
		[this](auto _)
	{
		
	});
	/// play event task
	m_Impl->m_View->OnPlayButtonClicked().subscribe(
		[this](auto _)
	{
		m_Impl->m_Executor->run(m_Impl->m_PlaybackTaskFlow).get();	
	});
	/// stop event task
	m_Impl->m_View->OnStopButtonClicked().subscribe(
		[this](auto _)
	{
		//m_Impl->m_Model->Stop();
	});
	/// seek forward event task
	m_Impl->m_View->OnSeekForwardButtonClicked().subscribe(
		[this](auto _)
	{

	});

	m_Impl->m_View->Deactivate();
	m_Impl->m_Model->Initialize();
}
///	\brief destroy the presenter
///	destroys the decoding context
void PlayerControllerPresenter::Destroy()
{
	m_Impl->m_Model->Terminate();
}
}	///	!namespace fusion
}	///	!namespace fu