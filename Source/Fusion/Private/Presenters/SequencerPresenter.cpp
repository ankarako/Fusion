#include <Presenters/SequencerPresenter.h>
#include <Views/SequencerView.h>
#include <Models/PlayerModel.h>
#include <WidgetRepo.h>

namespace fu {
namespace fusion {

struct SequencerPresenter::Impl
{
	view_ptr_t				m_View;
	wrepo_ptr_t				m_Wrepo;
	player_ptr_t			m_PlayerModel;
	perfcap_player_ptr_t	m_PerfcapPlayer;

	Impl(view_ptr_t view, wrepo_ptr_t wrepo, player_ptr_t player, perfcap_player_ptr_t perfcap_player)
		: m_View(view), m_Wrepo(wrepo), m_PlayerModel(player), m_PerfcapPlayer(perfcap_player)
	{ }
};	///	!struct Impl
/// Construction
SequencerPresenter::SequencerPresenter(view_ptr_t view, wrepo_ptr_t wrepo, player_ptr_t player, perfcap_player_ptr_t perfcap_player)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, wrepo, player, perfcap_player))
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

	//m_Impl->m_View->OnAnimationStartPlayback()
	m_Impl->m_View->OnVideoStartPlayback()
		.subscribe([this](auto) 
	{
		//m_Impl->m_PlayerModel->Start();
	});

	m_Impl->m_View->OnPauseButtonClicked()
		.subscribe([this](auto) 
	{
		//m_Impl->m_PlayerModel->Pause();
	});

	m_Impl->m_View->OnStopButtonClicked()
		.subscribe([this](auto _) 
	{
		//m_Impl->m_PlayerModel->Stop();
	});
	m_Impl->m_View->Activate();
}

}	///	@namesapce fusion
}	///	!namesapce fu