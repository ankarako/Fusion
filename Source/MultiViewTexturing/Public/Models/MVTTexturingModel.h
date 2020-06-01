#ifndef	__MVT_PUVLIC_MODELS_MVTTEXTURINGMODEL_H__
#define __MVT_PUVLIC_MODELS_MVTTEXTURINGMODEL_H__

#include <Initializable.h>
#include <MeshData.h>
#include <VolcapCameraData.h>
#include <PerfcapAnimationData.h>
#include <TrackedData.h>
#include <Buffer.h>
#include <vector>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace mvt {
class MVTModel : public app::Initializable
{
public:
	MVTModel();
	void Init() override;

	rxcpp::observer<io::MeshData>							MeshDataFlowIn();
	rxcpp::observable<io::MeshData>							MeshDataFlowOut();
	rxcpp::observer<std::vector<io::volcap_cam_data_ptr_t>> CameraDataFlowIn();
	rxcpp::observable<std::vector<io::volcap_cam_data_ptr_t>> CameraDataFlowOut();

	rxcpp::observer<std::vector<BufferCPU<uchar4>>>			CameraFramesFlowIn();
	rxcpp::observer<io::tracked_seq_ptr_t>					TrackedFramesFlowIn();
	rxcpp::observer<io::perfcap_skeleton_ptr_t>				SkeletonFlowIn();
	rxcpp::observer<io::perfcap_skin_data_ptr_t>			SkinningDataFlowIn();
	rxcpp::observer<void*>									RunTexturingLoop();
	rxcpp::observable<int>									SeekFrameFlowOut();
	rxcpp::observable<BufferCPU<uchar4>>					TextureFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class MVTModel
}
}	///	!namespace fu
#endif	///	!__MVT_PUVLIC_MODELS_MVTTEXTURINGMODEL_H__
