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
	///=================
	/// Skeleton Input
	///================
	m_Impl->m_PerformanceImportModel->SkeletonFlowOut()
		.subscribe(m_Impl->m_Model->SkeletonFlowIn());
	///====================
	/// Skinning Data Input
	///====================
	m_Impl->m_PerformanceImportModel->SkinDataFlowOut()
		.subscribe(m_Impl->m_Model->SkinningDataFlowIn());
	///===================
	/// Seek Frame Output
	///===================
	m_Impl->m_Model->SeekFrameFlowOut()
		.subscribe(m_Impl->m_MultiViewPlayerModel->SeekFrameIdFlowIn());

	m_Impl->m_Model->CameraDataFlowOut()
		.subscribe(m_Impl->m_ViewportTracingModel->CameraDataFlowIn());

	m_Impl->m_Model->MeshDataFlowOut()
		.subscribe(m_Impl->m_ViewportTracingModel->MeshDataFlowIn());

	m_Impl->m_Model->AnimatedMeshDataFlowOut()
		.subscribe(m_Impl->m_ViewportTracingModel->AnimatedMeshDataFlowIn());
	///===============
	/// texture Output
	///===============
	m_Impl->m_Model->TextureFlowOut()
		.subscribe(m_Impl->m_ViewportTracingModel->TriangleMeshTextureFlowIn());
	///================
	/// Camera matrices
	///================
	m_Impl->m_Model->CameraMatricesFlowOut()
		.subscribe(m_Impl->m_MultiViewPlayerModel->CameraMatricesFlowIn());
	///===============================
	/// Distortion Coefficients Output
	///===============================
	m_Impl->m_Model->DistortionCoefficientsFlowOut()
		.subscribe(m_Impl->m_MultiViewPlayerModel->DistortionCoefficientsFlowIn());
	///===========================================
	m_Impl->m_Model->SetOutputDir("output");
	m_Impl->m_Model->SetExportDir("export");
	m_Impl->m_Model->SetTextureSize(make_uint2(2048, 2048));
	m_Impl->m_Model->SetSeparateTextures(false);
	m_Impl->m_Model->SetLaunchMult(4);
	m_Impl->m_Model->SetViewportEnabled(false);
	m_Impl->m_MultiViewPlayerModel->SetDebugFramesEnabled(false);
	m_Impl->m_MultiViewPlayerModel->SetUndistortEnabled(true);
	m_Impl->m_Model->SetTempFolderPath(m_Impl->m_PerformanceImportModel->TempFolderPath());
	m_Impl->m_Model->SetSkeletonFilename(m_Impl->m_PerformanceImportModel->SkeletonFilename());
	m_Impl->m_Model->SetSkinningFilename(m_Impl->m_PerformanceImportModel->SkinningFilename());
	m_Impl->m_Model->SetTemplateMeshFilename(m_Impl->m_PerformanceImportModel->TemplateMeshFilename());
	m_Impl->m_Model->SetTrackedParamsFilename(m_Impl->m_PerformanceImportModel->TrackedParamsFilename());
}
}	///	!namespace mvt
}	///	!namespace fu