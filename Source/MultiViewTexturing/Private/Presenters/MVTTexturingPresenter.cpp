#include <Presenters/MVTTexturingPresenter.h>
#include <Models/MVTTexturingModel.h>
#include <Models/PerformanceImportModel.h>
#include <Models/ViewportTracingModel.h>
#include <Models/MultiViewPlayerModel.h>

namespace fu {
namespace mvt {
struct MVTPresenter::Impl
{
	model_ptr_t 			m_Model;
	perf_import_model_ptr_t	m_PerformanceImportModel;
	viewport_model_ptr_t	m_ViewportTracingModel;
	mv_player_model_ptr_t	m_MultiViewPlayerModel;

	Impl(model_ptr_t model, perf_import_model_ptr_t perf_import_model, viewport_model_ptr_t viewport_model, mv_player_model_ptr_t mv_player_model)
		: m_Model(model)
		, m_PerformanceImportModel(perf_import_model)
		, m_ViewportTracingModel(viewport_model)
		, m_MultiViewPlayerModel(mv_player_model)
	{ }
};
/// Construction
MVTPresenter::MVTPresenter(model_ptr_t model, perf_import_model_ptr_t perf_import_model, viewport_model_ptr_t viewport_model, mv_player_model_ptr_t mv_player_model)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, perf_import_model, viewport_model, mv_player_model))
{ }
///
void MVTPresenter::Init()
{
	///==============
	/// Mesh Input
	///=============
	m_Impl->m_PerformanceImportModel->MeshDataFlowOut()
		.subscribe(m_Impl->m_Model->MeshDataFlowIn());
	///==================
	/// CameraData Input
	///==================
	m_Impl->m_PerformanceImportModel->ViewportDataFlowOut()
		.subscribe(m_Impl->m_Model->CameraDataFlowIn());
	///===================
	/// Video Frames Input
	///===================
	m_Impl->m_MultiViewPlayerModel->MultiViewFramesFlowOut()
		.subscribe(m_Impl->m_Model->CameraFramesFlowIn());
	///=========================
	///	Tracked Parameters Input
	///=========================
	m_Impl->m_PerformanceImportModel->TrackedSequenceDataFlowOut()
		.subscribe(m_Impl->m_Model->TrackedFramesFlowIn());
	///===================
	/// Seek Frame Output
	///===================
	m_Impl->m_Model->SeekFrameFlowOut()
		.subscribe(m_Impl->m_MultiViewPlayerModel->SeekFrameIdFlowIn());

	m_Impl->m_Model->CameraDataFlowOut()
		.subscribe(m_Impl->m_ViewportTracingModel->CameraDataFlowIn());

	m_Impl->m_Model->MeshDataFlowOut()
		.subscribe(m_Impl->m_ViewportTracingModel->MeshDataFlowIn());
	///===============
	/// texture Output
	///===============
	m_Impl->m_Model->TextureFlowOut()
		.subscribe(m_Impl->m_ViewportTracingModel->TriangleMeshTextureFlowIn());
}
}	///	!namespace mvt
}	///	!namespace fu