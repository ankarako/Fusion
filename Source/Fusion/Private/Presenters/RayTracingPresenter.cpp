#include <Presenters/RayTracingPresenter.h>
#include <Models/RayTracingModel.h>
#include <Views/FileExplorerView.h>
#include <Views/RayTracingView.h>
#include <Core/Coordination.h>
#include <Settings/RayTracingSettings.h>
#include <Core/SettingsRepo.h>
#include <Models/DepthEstimationModel.h>
#include <Models/AssetLoadingModel.h>
#include <Views/RayTracingControlView.h>
#include <Views/SequencerView.h>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief RayTRacingPresenter's implementation
struct RayTracingPresenter::Impl
{
	using ray_set_ptr_t = std::shared_ptr<RayTracingSettings>;

	fexp_view_ptr_t		m_FexpView;
	model_ptr_t			m_Model;
	view_ptr_t			m_View;
	wrepo_ptr_t			m_WRepo;
	coord_ptr_t			m_Coord;
	ray_set_ptr_t		m_Settings;
	srepo_ptr_t			m_Srepo;
	dest_model_ptr_t	m_DepthEstModel;
	rt_ctrl_view_ptr_t	m_RayControlView;
	asset_model_ptr_t	m_AssetModel;
	seq_view_ptr_t		m_SequencerView;
	/// Construction
	Impl(fexp_view_ptr_t fexpView, 
		model_ptr_t model, 
		view_ptr_t view, 
		wrepo_ptr_t wrepo, 
		coord_ptr_t coord, 
		srepo_ptr_t	srepo,
		dest_model_ptr_t dest_model,
		rt_ctrl_view_ptr_t rt_ctrl_view,
		asset_model_ptr_t asset_model,
		seq_view_ptr_t seq_view)
		: m_FexpView(fexpView)
		, m_Model(model)
		, m_View(view)
		, m_WRepo(wrepo)
		, m_Coord(coord)
		, m_Srepo(srepo)
		, m_DepthEstModel(dest_model)
		, m_RayControlView(rt_ctrl_view)
		, m_AssetModel(asset_model)
		, m_SequencerView(seq_view)
	{ }
};	///	!struct Impl
/// Construction
RayTracingPresenter::RayTracingPresenter(
	fexp_view_ptr_t fexpView, 
	model_ptr_t model, 
	view_ptr_t view, 
	wrepo_ptr_t wrepo, 
	coord_ptr_t coord, 
	srepo_ptr_t srepo,
	dest_model_ptr_t dest_model,
	rt_ctrl_view_ptr_t rt_ctrl_view,
	asset_model_ptr_t asset_model,
	seq_view_ptr_t seq_view)
	: m_Impl(spimpl::make_unique_impl<Impl>(fexpView, model, view, wrepo, coord, srepo, dest_model, rt_ctrl_view, asset_model, seq_view))
{ }
///	\brief initialize the presenter
///	Sets view model tasks
void RayTracingPresenter::Init()
{
	/// register our setttings
	m_Impl->m_Settings = std::make_shared<RayTracingSettings>();
	m_Impl->m_Srepo->RegisterSettings(m_Impl->m_Settings);
	///=====================
	///	View activation task
	///=====================
	m_Impl->m_View->OnActivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_WRepo->RegisterWidget(m_Impl->m_View);
	});
	///=======================
	/// View deactivation task
	///=======================
	m_Impl->m_View->OnDeactivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_WRepo->UnregisterWidget(m_Impl->m_View);
	});

	m_Impl->m_RayControlView->OnActivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_WRepo->RegisterWidget(m_Impl->m_RayControlView);
	});

	m_Impl->m_RayControlView->OnDeactivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_WRepo->UnregisterWidget(m_Impl->m_RayControlView);
	});
	///==================
	/// Load 3D file task
	///==================
	m_Impl->m_FexpView->Open3DFileFlowOut().observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe(m_Impl->m_AssetModel->FilepathFlowIn());
	///===============
	/// Asset flow out
	///===============
	m_Impl->m_AssetModel->MeshDataFlowOut()
		.subscribe(m_Impl->m_Model->MeshDataFlowIn());
	///=================
	/// Settings loaded
	///=================
	m_Impl->m_Settings->OnSettingsLoaded()
		.subscribe([this](auto _) 
	{
		if (!m_Impl->m_Settings->LoadedFiles.empty())
		{
			for (int i = 0; i < m_Impl->m_Settings->LoadedFiles.size(); i++)
			{
				m_Impl->m_Model->SetIsValid(false);
				/// TODO: Laod Asset
			}
		}
	});

	///============================
	/// viewport size changed task
	///===========================
	m_Impl->m_View->OnViewportSizeChanged().subscribe(
		[this](float2 size) 
	{
		uint2 s = make_uint2((unsigned int)size.x, (unsigned int)size.y);
		m_Impl->m_Model->LaunchSizeFlowIn().on_next(s);
	});
	///=================================
	///	tracing  context frame flow out
	///=================================
	m_Impl->m_Model->FrameFlowOut()
		.observe_on(m_Impl->m_Coord->UICoordination())
		.subscribe(m_Impl->m_View->FrameFlowIn());
	///=================================
	/// camera rotation output to input
	///=================================
	m_Impl->m_View->RotationTransformFlowOut()
		.subscribe(m_Impl->m_Model->CameraRotationTransformFlowIn());
	///=================================
	/// camera translation output to input
	///=================================
	m_Impl->m_View->TranslationFlowOut()
		.subscribe(m_Impl->m_Model->CameraTranslationFlowIn());
	///=====================================
	/// Depth estimation made a point cloud
	///=====================================
	m_Impl->m_DepthEstModel->PointCloudFilepathFlowOut()
		.subscribe(m_Impl->m_AssetModel->FilepathFlowIn());
	///========================================
	/// Normals Flow In from normals estimation
	///=========================================
	m_Impl->m_RayControlView->OnCullingPlanePositionChanged()
		.subscribe(m_Impl->m_Model->CullingPlanePositionFlowIn());
	/// 
	m_Impl->m_RayControlView->OnPclSizeChanged()
		.subscribe(m_Impl->m_Model->PointcloudPointSizeFlowIn());
	/// activate 3D viewport
	m_Impl->m_View->Activate();
}
}	///	!namespace fusion
}	///	!namespace fu