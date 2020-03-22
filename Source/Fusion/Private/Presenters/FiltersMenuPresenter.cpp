#include <Presenters/FiltersMenuPresenter.h>
#include <WidgetRepo.h>
#include <Views/MainToolbarView.h>
#include <Views/DepthEstimationSettingsView.h>
#include <Models/PlayerModel.h>
#include <Models/ProjectModel.h>
#include <Models/DepthEstimationModel.h>
#include <Models/NormalsEstimationModel.h>
#include <Core/Coordination.h>

namespace fu {
namespace fusion {
struct FiltersMenuPresenter::Impl
{
	view_ptr_t				m_View;
	player_model_ptr_t		m_PlayerModel;
	prj_model_ptr_t			m_ProjectModel;
	depth_est_model_ptr_t	m_DepthEstModel;
	depth_est_view_ptr_t	m_DepthEstSetView;
	normal_est_model_ptr_t	m_NormalEstModel;
	wrepo_ptr_t				m_Wrepo;
	coord_ptr_t				m_Coord;

	Impl(view_ptr_t				view,
		player_model_ptr_t		player_model,
		prj_model_ptr_t			prj_model,
		depth_est_model_ptr_t	depth_est_model,
		depth_est_view_ptr_t	depth_est_view,
		normal_est_model_ptr_t	normal_est_model,
		wrepo_ptr_t				wrepo,
		coord_ptr_t				coord)
		: m_View(view)
		, m_PlayerModel(player_model)
		, m_ProjectModel(prj_model)
		, m_DepthEstModel(depth_est_model)
		, m_DepthEstSetView(depth_est_view)
		, m_NormalEstModel(normal_est_model)
		, m_Wrepo(wrepo)
		, m_Coord(coord)
	{ }
};
/// Construction
FiltersMenuPresenter::FiltersMenuPresenter(
	view_ptr_t				view,
	player_model_ptr_t		player_model,
	prj_model_ptr_t			prj_model,
	depth_est_model_ptr_t	depth_est_model,
	depth_est_view_ptr_t	depth_est_view,
	normal_est_model_ptr_t	normal_est_model,
	wrepo_ptr_t				wrepo,
	coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, player_model, prj_model, depth_est_model, depth_est_view, normal_est_model, wrepo, coord))
{ }

void FiltersMenuPresenter::Init()
{
	m_Impl->m_View->OnFiltersMenu_EstimateDepthClicked()
		.subscribe([this](auto _) 
	{
		m_Impl->m_DepthEstSetView->Activate();
	});


	m_Impl->m_DepthEstSetView->OnCancelButtonClicked()
		.subscribe([this](auto _)
	{
		m_Impl->m_DepthEstSetView->Deactivate();
	});

	m_Impl->m_DepthEstSetView->OnDepthEstimationModelChanged()
		.subscribe([this](DepthEstimationModels model)
	{

	});

	m_Impl->m_DepthEstSetView->OnEstimateDepthButtonClicked().observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](auto _)
	{
		m_Impl->m_DepthEstSetView->Deactivate();
		/// send buffer to depth est model ->
		uint2 framesize = m_Impl->m_PlayerModel->GetFrameSize();
		BufferCPU<uchar4> curFrame = m_Impl->m_PlayerModel->GetCurrentFrame();
		auto sizeFramePair = std::make_pair(framesize, curFrame);
		m_Impl->m_DepthEstModel->FrameFlowIn().on_next(sizeFramePair);
	});

	m_Impl->m_View->OnFiltersMenu_EstimateNormalsClicked().observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](auto _) 
	{
		/// send buffer to depth est model ->
		uint2 framesize = m_Impl->m_PlayerModel->GetFrameSize();
		BufferCPU<uchar4> curFrame = m_Impl->m_PlayerModel->GetCurrentFrame();
		auto sizeFramePair = std::make_pair(framesize, curFrame);
		m_Impl->m_NormalEstModel->FrameFlowIn().on_next(sizeFramePair);
	});
		
}
}	///	!namespace fusion
}	///	!namespace fu