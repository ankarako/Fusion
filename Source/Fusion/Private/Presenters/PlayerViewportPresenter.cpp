#include <Presenters/PlayerViewportPresenter.h>
#include <Views/PlayerViewportView.h>
#include <Models/PlayerModel.h>
#include <Models/VideoTracingModel.h>
#include <WidgetRepo.h>
#include <Core/Coordination.h>
#include <Views/FileExplorerView.h>
#include <Models/AssetLoadingModel.h>
#include <Models/PerfcapPlayerModel.h>
#include <Models/FusedExportModel.h>
///
#include <plog/Log.h>

namespace fu {
namespace fusion {
struct PlayerViewportPresenter::Impl
{
	bool m_PlayingEnabled = true;
	model_ptr_t			m_Model;
	view_ptr_t			m_View;
	tracer_model_ptr_t	m_TracerModel;
	wrepo_ptr_t			m_Wrepo;
	coord_ptr_t			m_Coord;
	fexp_view_ptr_t		m_FexpView;
	asset_model_ptr_t	m_AssetModel;
	perfcap_model_ptr_t	m_PerfcapModel;
	fused_export_model_ptr_t m_FusedExportModel;
	rxcpp::composite_subscription m_TracerSub;

	Impl(model_ptr_t model, view_ptr_t view, tracer_model_ptr_t tracer_model, wrepo_ptr_t wrepo, coord_ptr_t coord, fexp_view_ptr_t fexp_view, asset_model_ptr_t asset_model, perfcap_model_ptr_t perfcap_model, fused_export_model_ptr_t fused_export_model)
		: m_Model(model), m_View(view), m_TracerModel(tracer_model), m_Wrepo(wrepo), m_Coord(coord), m_FexpView(fexp_view), m_AssetModel(asset_model), m_PerfcapModel(perfcap_model), m_FusedExportModel(fused_export_model)
	{ }
};
/// Construction
PlayerViewportPresenter::PlayerViewportPresenter(
	model_ptr_t model, 
	view_ptr_t view, 
	tracer_model_ptr_t tracer_model, 
	wrepo_ptr_t wrepo, 
	coord_ptr_t coord, 
	fexp_view_ptr_t fexp_view,
	asset_model_ptr_t asset_model,
	perfcap_model_ptr_t perfcap_model,
	fused_export_model_ptr_t fused_export_model)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, view, tracer_model, wrepo, coord, fexp_view, asset_model, perfcap_model, fused_export_model))
{ }

void PlayerViewportPresenter::Init()
{
	///=======================
	/// Widget activation task
	///=======================
	m_Impl->m_View->OnActivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_View);
	});
	///=========================
	/// Widget deactivation task
	///==========================
	m_Impl->m_View->OnDeactivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_View);
	});
	///========================
	/// frame size subscription
	///========================
	m_Impl->m_Model->FrameSizeFlowOut()
		//.observe_on(m_Impl->m_Coord->UICoordination())
		.subscribe(m_Impl->m_TracerModel->FrameSizeFlowIn());

	m_Impl->m_Model->FrameSizeFlowOut()
		.observe_on(m_Impl->m_Coord->UICoordination())
		.subscribe(m_Impl->m_View->FrameSizeFlowIn());
	///============================
	///	viewport size changed task
	///============================
	m_Impl->m_View->OnViewportSizeChanged()
		.subscribe([this](float2 newSize)
	{
		uint2 size = make_uint2((unsigned int)newSize.x, (unsigned int)newSize.y);
		m_Impl->m_TracerModel->FrameSizeFlowIn().on_next(size);
		m_Impl->m_View->FrameSizeFlowIn().on_next(size);
		m_Impl->m_Model->SetScalingSize(size);
	});
	///======================
	/// 3D file loading task
	///======================
	m_Impl->m_AssetModel->MeshDataFlowOut()
		.subscribe(m_Impl->m_TracerModel->MeshDataFlowIn());
	///=================================
	/// frame flow out from decoder task
	///=================================
	m_Impl->m_Model->CurrentFrameFlowOut()
		//.observe_on(m_Impl->m_Coord->UICoordination())
		.subscribe(m_Impl->m_TracerModel->FrameFlowIn());
	///================================
	/// frame flow out from tracer task
	///================================
	//m_Impl->m_TracerSub.add(
	m_Impl->m_TracerModel->FrameFlowOut()
		.observe_on(m_Impl->m_Coord->UICoordination())
		.subscribe(m_Impl->m_View->FrameFlowIn());
	//);

	m_Impl->m_View->Activate();

	m_Impl->m_FusedExportModel->StartedExportingFlowOut()
		.subscribe([this](auto _) 
	{
		//m_Impl->m_TracerSub.clear();
	});
}
}	///	!namespace fusion
}	///	!namespace fu