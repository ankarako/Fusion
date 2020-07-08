#ifndef	__MVT_PUVLIC_MODELS_MVTTEXTURINGMODEL_H__
#define __MVT_PUVLIC_MODELS_MVTTEXTURINGMODEL_H__

#include <Initializable.h>
#include <Destroyable.h>
#include <MeshData.h>
#include <VolcapCameraData.h>
#include <PerfcapAnimationData.h>
#include <TrackedData.h>
#include <Buffer.h>
#include <vector>
#include <spimpl.h>
#include <DistortionCoefficients.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace mvt {
class MVTModel : public app::Initializable, public app::Destroyable
{
public:
	MVTModel();
	/// Derived
	void Init() override;
	void Destroy() override;
	/// Interface
	void SetOutputDir(const std::string& dir);
	void SetExportDir(const std::string& dir);
	void SetTextureSize(const uint2& size);
	void SetSeparateTextures(bool set);
	void SetLaunchMult(int mult);
	void SetViewportEnabled(bool enabled);
	void SetDeleteOutputEnabled(bool enabled);
	void SetTempFolderPath(const std::string& filename);
	void SetSkinningFilename(const std::string& filename);
	void SetSkeletonFilename(const std::string& filename);
	void SetTrackedParamsFilename(const std::string& filename);
	void SetTemplateMeshFilename(const std::string& filename);
	void SetSaveAnimatedMeshEnabled(const bool enabled);
	void SetExtraDataDir(const std::string& dir);
	rxcpp::observer<io::MeshData>							MeshDataFlowIn();
	rxcpp::observable<io::MeshData>							MeshDataFlowOut();
	rxcpp::observable<io::MeshData>							AnimatedMeshDataFlowOut();
	rxcpp::observer<std::vector<io::volcap_cam_data_ptr_t>> CameraDataFlowIn();
	rxcpp::observable<std::vector<io::volcap_cam_data_ptr_t>> CameraDataFlowOut();

	rxcpp::observer<std::vector<BufferCPU<uchar4>>>			CameraFramesFlowIn();
	rxcpp::observer<io::tracked_seq_ptr_t>					TrackedFramesFlowIn();
	rxcpp::observer<io::perfcap_skeleton_ptr_t>				SkeletonFlowIn();
	rxcpp::observer<io::perfcap_skin_data_ptr_t>			SkinningDataFlowIn();
	rxcpp::observer<void*>									RunTexturingLoop();
	rxcpp::observable<int>									SeekFrameFlowOut();
	rxcpp::observable<BufferCPU<uchar4>>					TextureFlowOut();
	rxcpp::observable<std::vector<BufferCPU<float>>>		CameraMatricesFlowOut();
	rxcpp::observable<std::vector<DistCoeffs>>				DistortionCoefficientsFlowOut();
	rxcpp::observable<unsigned int>							FrameCountFlowOut();
	rxcpp::observable<void*>								ProgressTickFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class MVTModel
}
}	///	!namespace fu
#endif	///	!__MVT_PUVLIC_MODELS_MVTTEXTURINGMODEL_H__
