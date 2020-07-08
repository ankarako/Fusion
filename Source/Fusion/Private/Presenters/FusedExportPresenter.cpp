#include <Presenters/FusedExportPresenter.h>
#include <Models/FusedExportModel.h>
#include <Models/PlayerModel.h>
#include <Models/PerfcapPlayerModel.h>
#include <Models/VideoTracingModel.h>
#include <Views/FileExplorerView.h>
#include <Core/Coordination.h>

namespace fu {
namespace fusion {
struct FusedExportPresenter::Impl
{
	bool						m_ExportingEnabled{ false };
	model_ptr_t 				m_Model;
	player_model_ptr_t 			m_PlayerModel;
	perfcap_player_model_ptr_t 	m_PerfcapPlayer;
	video_tracing_model_ptr_t	m_VideoTracingModel;
	fexp_view_ptr_t				m_FexpView;
	coord_ptr_t					m_Coord;

	Impl(model_ptr_t model, 
		player_model_ptr_t player_model, 
		perfcap_player_model_ptr_t perfcap_player_model, 
		video_tracing_model_ptr_t video_tracing_model,
		fexp_view_ptr_t fexp_view,
		coord_ptr_t coord)
		: m_Model(model)
		, m_PlayerModel(player_model)
		, m_PerfcapPlayer(perfcap_player_model)
		, m_VideoTracingModel(video_tracing_model)
		, m_FexpView(fexp_view)
		, m_Coord(coord)
	{ }
};	///	!struct Impl

FusedExportPresenter::FusedExportPresenter(
	model_ptr_t model, 
	player_model_ptr_t player_model, 
	perfcap_player_model_ptr_t perfcap_player_model, 
	video_tracing_model_ptr_t video_tracing_model,
	fexp_view_ptr_t fexp_view,
	coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, player_model, perfcap_player_model, video_tracing_model, fexp_view, coord))
{ }

void FusedExportPresenter::Init()
{
	///======================================
	/// Exporting Fused Video initialization
	///======================================
	m_Impl->m_FexpView->ExportFusedVideoFilepathFlowOut()
		.observe_on(m_Impl->m_Coord->ModelCoordination())
		.map([this](const std::string& filepath) 
	{ 
		m_Impl->m_ExportingEnabled = true; 
		return filepath; 
	}).subscribe(m_Impl->m_Model->ExportFilepathFlowIn());
	///======================================
	/// Exporting Mentor Layer initialization
	///======================================
	m_Impl->m_FexpView->ExportMentorLayerFilepathFlowOut()
		.observe_on(m_Impl->m_Coord->ModelCoordination())
		.map([this](const std::string& filepath)
	{
		m_Impl->m_ExportingEnabled = true;
		return filepath;
	}).subscribe(m_Impl->m_Model->ExportMentorLayerFilepathFlowIn());


	m_Impl->m_Model->StartedExportingMentorLayer()
		.subscribe(m_Impl->m_PlayerModel->ChromaKeyBGFlowIn());


	m_Impl->m_Model->FinishedExportingFlowOut()
		.subscribe([this](auto _) 
	{
		m_Impl->m_ExportingEnabled = false;
		m_Impl->m_PlayerModel->Stop();
		m_Impl->m_PerfcapPlayer->Stop();
	});

	m_Impl->m_Model->FinishedExportingMentorLayerFlowOut()
		.map([this](auto _) 
	{
		m_Impl->m_ExportingEnabled = false;
		m_Impl->m_PerfcapPlayer->Stop();
		return _;
	}).subscribe(m_Impl->m_PlayerModel->RemoveChromaKeyBGFlowIn());
	///============================================
	/// Seek Frames on player and on perfcap player
	///============================================
	m_Impl->m_Model->SeekFrameIdFlowOut()
		.subscribe([this](int frameID) 
	{
		m_Impl->m_PerfcapPlayer->SeekFrame(frameID);
		m_Impl->m_PlayerModel->Seek(frameID);
	});

	///=========================================
	/// Get fused frame from video tracing model
	///=========================================
	m_Impl->m_VideoTracingModel->FrameFlowOut()
		.filter([this](const auto& frame) 
	{
		return m_Impl->m_ExportingEnabled;
	}).subscribe(m_Impl->m_Model->FusedFrameFlowIn());

	m_Impl->m_PlayerModel->SequenceItemFlowOut()
		.subscribe(m_Impl->m_Model->SequenceItemFlowIn());

	m_Impl->m_PerfcapPlayer->SequenceItemFlowOut()
		.subscribe(m_Impl->m_Model->SequenceItemFlowIn());
	

	///===========================
	/// Get animation frame count
	///===========================
	m_Impl->m_PerfcapPlayer->AnimationFramesCountFlowOut()
		.subscribe(m_Impl->m_Model->AnimatedFramesCountFlowIn());

	///=======================
	/// Get video frame size
	///=======================
	m_Impl->m_PlayerModel->FrameSizeFlowOut()
		.subscribe(m_Impl->m_Model->VideoSizeFlowIn());
}
}	///	!namespace fusion
}	///	!namespace fu