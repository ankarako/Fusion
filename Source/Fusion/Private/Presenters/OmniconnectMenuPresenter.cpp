#include <Presenters/OmniconnectMenuPresenter.h>
#include <Views/MainToolbarView.h>
#include <Views/ProgressBarView.h>
#include <Models/OmniConnectModel.h>
#include <Models/ProjectModel.h>
#include <Models/PlayerModel.h>
#include <Models/FusedExportModel.h>
#include <Core/Coordination.h>
#include <Views/OmniconnectVideoListView.h>
#include <WidgetRepo.h>

namespace fu {
namespace fusion {
struct OmniconnectMenuPresenter::Impl
{
	model_ptr_t			m_Model;
	toolbar_view_ptr_t	m_ToolbarView;
	prj_model_ptr_t		m_PrjModel;
	coord_ptr_t			m_Coord;
	wrepo_ptr_t			m_WidgetRepo;
	vlist_view_ptr_t	m_VideoListView;
	player_model_ptr_t	m_PlayerModel;
	prog_view_ptr_t		m_ProgressBarView;
	export_model_ptr_t	m_ExportModel;

	Impl(model_ptr_t model, 
		toolbar_view_ptr_t toolbar_view, 
		prj_model_ptr_t prj_model, 
		coord_ptr_t coord, 
		wrepo_ptr_t wrepo, 
		vlist_view_ptr_t vlist_view,
		player_model_ptr_t player_model,
		prog_view_ptr_t prog_view,
		export_model_ptr_t export_model)
		: m_Model(model)
		, m_ToolbarView(toolbar_view)
		, m_PrjModel(prj_model)
		, m_Coord(coord)
		, m_WidgetRepo(wrepo)
		, m_VideoListView(vlist_view)
		, m_PlayerModel(player_model)
		, m_ProgressBarView(prog_view)
		, m_ExportModel(export_model)
	{ }
};

OmniconnectMenuPresenter::OmniconnectMenuPresenter(model_ptr_t model, toolbar_view_ptr_t toolbar_view, prj_model_ptr_t prj_model, coord_ptr_t coord, wrepo_ptr_t wrepo, vlist_view_ptr_t vlist_view, player_model_ptr_t player_model, prog_view_ptr_t prog_view, export_model_ptr_t export_model)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, toolbar_view, prj_model, coord, wrepo, vlist_view, player_model, prog_view, export_model))

{ }

void OmniconnectMenuPresenter::Init()
{
	/// Video list view de/activation
	m_Impl->m_VideoListView->OnActivated()
		.subscribe([this](auto _) { m_Impl->m_WidgetRepo->RegisterWidget(m_Impl->m_VideoListView); });
	m_Impl->m_VideoListView->OnDeactivated()
		.subscribe([this](auto _) { m_Impl->m_WidgetRepo->UnregisterWidget(m_Impl->m_VideoListView); });
	///=================
	/// Project Settings
	///=================
	m_Impl->m_PrjModel->SettingsFlowOut()
		.subscribe(m_Impl->m_Model->ProjectSettingsFlowIn());

	///=================
	/// Show Video Lists
	///=================
	m_Impl->m_ToolbarView->OnOmniconnectMenu_ShowVideoListClicked()
		.observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe(m_Impl->m_Model->ShowMediaListFlowIn());
	///========================
	/// Video List View Output
	///========================
	m_Impl->m_Model->VideoListFlowOut()
		.tap([this](const omni_video_list_ptr_t& list) 
	{	
		if (!m_Impl->m_VideoListView->IsActive())
		{
			m_Impl->m_VideoListView->Activate();
		}
	}).subscribe(m_Impl->m_VideoListView->VideoListFlowIn());
	///========================
	/// Download Selected video
	///========================
	m_Impl->m_VideoListView->DownloadSelectedVideoIndexFlowOut()
		.tap([this](int idx) 
	{
		m_Impl->m_ProgressBarView->MessageFlowIn().on_next("Downloading Video");
		m_Impl->m_ProgressBarView->Activate();
	}).observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe(m_Impl->m_Model->VideoListIndexFlowIn());
	///==================
	/// Downloaded Video
	///==================
	m_Impl->m_Model->VideoFilepathFlowOut()
		.tap([this](const std::string& filepath) 
	{
		m_Impl->m_ProgressBarView->Deactivate();
	}).subscribe([this](const std::string& filepath) 
	{
		m_Impl->m_PlayerModel->LoadFile(filepath);
	});

	m_Impl->m_Model->ProgressFlowOut()
		.subscribe([this](float prog) 
	{
		m_Impl->m_ProgressBarView->ProgressFlowIn().on_next(prog);
	});
		
	m_Impl->m_ToolbarView->OnOmniconnectMenu_UploadMentorLayerClicked()
		.subscribe(m_Impl->m_Model->ExportMentorFileFlowIn());

	m_Impl->m_ExportModel->FinishedExportingMentorLayerFlowOut()
		.subscribe(m_Impl->m_Model->UploadMentorLayerFlowIn());
}
}	///	!namespace fusion
}	///	!namespace fu