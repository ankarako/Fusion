#include <Presenters/RayTracingPresenter.h>
#include <Models/RayTracingModel.h>
#include <Views/FileExplorerView.h>
#include <Views/RayTracingView.h>
#include <Core/Coordination.h>
#include <Systems/AssetLoadingSystem.h>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief RayTRacingPresenter's implementation
struct RayTracingPresenter::Impl
{
	fexp_view_ptr_t		m_FexpView;
	model_ptr_t			m_Model;
	view_ptr_t			m_View;
	wrepo_ptr_t			m_WRepo;
	coord_ptr_t			m_Coord;
	asset_syst_ptr_t	m_AssetSystem;
	/// Construction
	Impl(fexp_view_ptr_t fexpView, model_ptr_t model, view_ptr_t view, wrepo_ptr_t wrepo, coord_ptr_t coord, asset_syst_ptr_t asset_syst)
		: m_FexpView(fexpView)
		, m_Model(model)
		, m_View(view)
		, m_WRepo(wrepo)
		, m_Coord(coord)
		, m_AssetSystem(asset_syst)
	{ }
};	///	!struct Impl
/// Construction
RayTracingPresenter::RayTracingPresenter(fexp_view_ptr_t fexpView, model_ptr_t model, view_ptr_t view, wrepo_ptr_t wrepo, coord_ptr_t coord, asset_syst_ptr_t asset_syst)
	: m_Impl(spimpl::make_unique_impl<Impl>(fexpView, model, view, wrepo, coord, asset_syst))
{ }
///	\brief initialize the presenter
///	Sets view model tasks
void RayTracingPresenter::Init()
{
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
	///==================
	/// Load 3D file task
	///==================
	m_Impl->m_FexpView->Open3DFileFlowOut().observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](const std::string& filepath) 
	{
		m_Impl->m_Model->SetIsValid(false);
		m_Impl->m_AssetSystem->LoadAsset(filepath, m_Impl->m_Model->GetCtxComp());
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
	m_Impl->m_Model->FrameFlowOut().observe_on(m_Impl->m_Coord->UICoordination())
		.subscribe(m_Impl->m_View->FrameFlowIn());
	///=========================================================
	///	Asset loading system triangle mesh output to Model input
	///=========================================================
	m_Impl->m_AssetSystem->TriangleMeshFlowOut()
		.subscribe(m_Impl->m_Model->TriangleMeshCompFlowIn());
	///=========================================================
	///	asset loading system point cloud output to model input
	///=========================================================
	m_Impl->m_AssetSystem->PointCloudFlowOut()
		.subscribe(m_Impl->m_Model->PointCloudFlowIn());
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
	/// activate 3D viewport
	m_Impl->m_View->Activate();
}
}	///	!namespace fusion
}	///	!namespace fu