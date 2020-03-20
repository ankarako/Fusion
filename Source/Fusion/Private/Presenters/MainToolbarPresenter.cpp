#include <Presenters/MainToolbarPresenter.h>
#include <WidgetRepo.h>
#include <Views/MainToolbarView.h>
#include <Views/FileExplorerView.h>
#include <Views/DepthEstimationSettingsView.h>
#include <Models/DepthEstimationModel.h>
#include <Models/ProjectModel.h>
#include <Models/PlayerModel.h>
#include <Core/Coordination.h>
#include <Views/RayTracingControlView.h>
#include <Models/PerfcapPlayerModel.h>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief MainToolbarPresenter implementation
struct MainToolbarPresenter::Impl
{
	view_ptr_t			m_View;
	fexp_view_ptr_t		m_FexpView;
	wrepo_ptr_t			m_Wrepo;
	player_model_ptr_t	m_PlayerModel;
	prj_model_ptr_t		m_ProjectModel;
	dest_set_view_ptr_t m_DepthEstSetView;
	dest_model_ptr_t	m_DepthEstModel;
	coord_ptr_t			m_Coord;
	rt_ctrl_view_ptr_t	m_RayControlView;
	perfcap_model_ptr_t	m_PerfcapModel;

	Impl(player_model_ptr_t decoder_model, view_ptr_t view, fexp_view_ptr_t fexp_view, wrepo_ptr_t wrepo, prj_model_ptr_t prj_model, dest_set_view_ptr_t dest_view, dest_model_ptr_t dest_model, coord_ptr_t coord, rt_ctrl_view_ptr_t rt_ctrl_view, perfcap_model_ptr_t perfcap_model)
		: m_View(view), m_FexpView(fexp_view), m_Wrepo(wrepo), m_PlayerModel(decoder_model), m_ProjectModel(prj_model), m_DepthEstSetView(dest_view), m_DepthEstModel(dest_model), m_Coord(coord), m_RayControlView(rt_ctrl_view), m_PerfcapModel(perfcap_model)
	{ }
};	///	!struct Impl
///	Construction
MainToolbarPresenter::MainToolbarPresenter(
	player_model_ptr_t	dec_model, 
	view_ptr_t			view, 
	fexp_view_ptr_t		fexp_view, 
	wrepo_ptr_t			wrepo, 
	prj_model_ptr_t		prj_model,
	dest_set_view_ptr_t dest_view,
	dest_model_ptr_t	dest_model,
	coord_ptr_t			coord,
	rt_ctrl_view_ptr_t	rt_ctrl_view,
	perfcap_model_ptr_t perfcap_model)
	: m_Impl(spimpl::make_unique_impl<Impl>(dec_model, view, fexp_view, wrepo, prj_model, dest_view, dest_model, coord, rt_ctrl_view, perfcap_model))
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

	m_Impl->m_View->OnFileMenu_Open3DFileClicked().subscribe(
		[this](auto _) 
	{
		m_Impl->m_FexpView->SetMode(FileExplorerMode::Open3DFile);
		m_Impl->m_FexpView->Activate();
	});

	m_Impl->m_View->OnFileMenu_SaveProjectClicked()
		.subscribe([this](auto _) 
	{
		m_Impl->m_ProjectModel->Save();
	});

	m_Impl->m_View->OnActivated()
		.subscribe([this](auto _)
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_DepthEstSetView);
	});

	m_Impl->m_View->OnDeactivated()
		.subscribe([this](auto _) 
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_DepthEstSetView);
	});

	m_Impl->m_View->OnFiltersMenu_EstimateDepthClicked()
		.subscribe([this](auto _)
	{
		m_Impl->m_DepthEstSetView->Activate();
	});

	m_Impl->m_FexpView->SaveProjectFileFlowOut()
		.subscribe(m_Impl->m_ProjectModel->CreateNewProjectFlowIn());

	m_Impl->m_FexpView->OpenProjectFileFlowOut()
		.subscribe(m_Impl->m_ProjectModel->LoadProjectFlowIn());


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

	m_Impl->m_View->OnWindowsMenu_RayTracingControlClicked()
		.subscribe([this](auto _) 
	{
		if (m_Impl->m_RayControlView->IsActive())
		{
			m_Impl->m_RayControlView->Deactivate();
		}
		else
		{
			m_Impl->m_RayControlView->Activate();
		}	
	});

	m_Impl->m_View->OnFileMenu_OpenPerfcapFileClicked()
		.subscribe([this](auto _) 
	{
		m_Impl->m_FexpView->SetMode(FileExplorerMode::OpenPerfcapFile);
		m_Impl->m_FexpView->Activate();
	});

	m_Impl->m_FexpView->OpenPerfcapFileFlowOut()
		.subscribe(m_Impl->m_PerfcapModel->PerfcapFilepathFlowIn());

	m_Impl->m_View->Activate();
}
}	///	!namespace fusion
}	///	!namespace  fu