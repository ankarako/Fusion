#include <Presenters/SequencerPresenter.h>
#include <Views/SequencerView.h>
#include <Models/PlayerModel.h>
#include <Models/PerfcapPlayerModel.h>
#include <Models/RayTracingModel.h>
#include <Models/VideoTracingModel.h>
#include <Models/AnimationModel.h>
#include <WidgetRepo.h>

namespace fu {
namespace fusion {

struct SequencerPresenter::Impl
{
	view_ptr_t				m_View;
	wrepo_ptr_t				m_Wrepo;
	player_ptr_t			m_PlayerModel;
	perfcap_player_ptr_t	m_PerfcapPlayer;
	rt_model_ptr_t			m_RayTracingModel;
	vrt_model_ptr_t			m_VideoTracingModel;
	anim_model_ptr_t		m_AnimationModel;

	Impl(view_ptr_t view, wrepo_ptr_t wrepo, player_ptr_t player, perfcap_player_ptr_t perfcap_player, rt_model_ptr_t rt_model, vrt_model_ptr_t vrt_model, anim_model_ptr_t anim_model)
		: m_View(view)
		, m_Wrepo(wrepo)
		, m_PlayerModel(player)
		, m_PerfcapPlayer(perfcap_player)
		, m_RayTracingModel(rt_model)
		, m_VideoTracingModel(vrt_model)
		, m_AnimationModel(anim_model)
	{ }
};	///	!struct Impl
/// Construction
SequencerPresenter::SequencerPresenter(
	view_ptr_t view, 
	wrepo_ptr_t wrepo, 
	player_ptr_t player, 
	perfcap_player_ptr_t perfcap_player,
	rt_model_ptr_t rt_model,
	vrt_model_ptr_t vrt_model,
	anim_model_ptr_t anim_model)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, wrepo, player, perfcap_player, rt_model, vrt_model, anim_model))
{ }

void SequencerPresenter::Init()
{
	///===================
	/// view activation
	///=================
	m_Impl->m_View->OnActivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_View);
	});
	///===================
	/// view deactivation
	///===================
	m_Impl->m_View->OnDeactivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_View);
	});
	///==============================
	/// PlayerModel Loaded video Task
	///==============================
	m_Impl->m_PlayerModel->SequenceItemFlowOut()
		.map([this](SequenceItem& item)
	{
		///===============
		/// Playback task
		///===============
		item.OnStartPlayback.get_observable().as_dynamic()
			.subscribe([this](auto _)
		{
			m_Impl->m_PlayerModel->Start();
		});
		///============
		///	Pause Task
		///============
		item.OnPause.get_observable().as_dynamic()
			.subscribe([this](auto _) 
		{
			m_Impl->m_PlayerModel->Pause();
		});
		///===========
		/// Stop Task
		///===========
		item.OnStop.get_observable().as_dynamic()
			.subscribe([this](auto _) 
		{
			m_Impl->m_PlayerModel->Stop();
		});
		///==============
		/// Seek forward
		///=============
		item.OnSeekForward.get_observable().as_dynamic()
			.subscribe([this](auto _)
		{
			m_Impl->m_PlayerModel->SeekForward();
		});
		///==============
		/// Seek forward
		///=============
		item.OnSeekBackward.get_observable().as_dynamic()
			.subscribe([this](auto _)
		{
			m_Impl->m_PlayerModel->SeekBackward();
		});
		///======
		///	Seek
		///======
		item.OnSeekFrame.get_observable().as_dynamic()
			.subscribe([this](int idx) 
		{
			m_Impl->m_PlayerModel->Seek(idx);
		});
		return item;
	}).subscribe(m_Impl->m_View->SequencerItemFlowIn());

	

	///======================
	/// sequence Loaded task
	///======================
	m_Impl->m_PerfcapPlayer->SequenceItemFlowOut()
		.map([this](SequenceItem& item) 
	{
		///===============
		/// Start Playback
		///===============
		item.OnStartPlayback.get_observable().as_dynamic()
			.subscribe([this](auto _) 
		{
			m_Impl->m_PerfcapPlayer->Start();
		});
		///======
		/// Pause
		///======
		item.OnPause.get_observable().as_dynamic()
			.subscribe([this](auto _) 
		{
			m_Impl->m_PerfcapPlayer->Pause();
		});
		///======
		/// Stop
		///=====
		item.OnStop.get_observable().as_dynamic()
			.subscribe([this](auto _) 
		{
			m_Impl->m_PerfcapPlayer->Stop();
		});
		///===========
		/// Sekk Back
		///===========
		item.OnSeekBackward.get_observable().as_dynamic()
			.subscribe([this](auto _) 
		{
			m_Impl->m_PerfcapPlayer->SeekBackward();
		});
		///===========
		/// Seek For
		///==========
		item.OnSeekForward.get_observable().as_dynamic()
			.subscribe([this](auto _) 
		{
			m_Impl->m_PerfcapPlayer->SeekForward();
		});
		return item;
	}).subscribe(m_Impl->m_View->SequencerItemFlowIn());
	///==========================================================
	/// Animation Frame flow out -> AnimationModel frame flow in
	///===========================================================
	m_Impl->m_PerfcapPlayer->AnimationFrameFlowOut()
		.subscribe(m_Impl->m_AnimationModel->AnimationFrameFlowIn());
	///=====================================================================
	/// Template mesh ptr flow out -> AnimationModel::template mesh flow in
	///=====================================================================
	m_Impl->m_PerfcapPlayer->TemplateMeshFlowOut()
		.subscribe(m_Impl->m_AnimationModel->TemplateMeshFlowIn());

	m_Impl->m_View->Activate();
}

}	///	@namesapce fusion
}	///	!namesapce fu