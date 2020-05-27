#ifndef __MVT_PUBLIC_MODELS_VIEWPORTTRACINGMODEL_H__
#define __MVT_PUBLIC_MODELS_VIEWPORTTRACINGMODEL_H__

#include <Initializable.h>
#include <Destroyable.h>
#include <VolcapCameraData.h>
#include <Buffer.h>
#include <MeshData.h>
#include <vector>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace mvt {
///	\class ViewportTracingModel
///	\brief ray casting for viewport visualization
class ViewportTracingModel : public app::Initializable, public app::Destroyable
{
public:
	using viewport_size_t = std::array<int, 2>;
	using mat_t = std::array<float, 16>;
	using trans_vec_t = std::array<float, 3>;
	/// Construction
	///	\brief default constructor (no dependencies)
	ViewportTracingModel();
	///	\brief model initialization
	void Init() override;
	///	\brief model destruction
	void Destroy() override;
	/// IO
	rxcpp::observer<viewport_size_t>						ViewportSizeFlowIn();
	rxcpp::observer<std::vector<io::volcap_cam_data_ptr_t>>	CameraDataFlowIn();
	rxcpp::observer<io::MeshData>							MeshDataFlowIn();
	rxcpp::observer<mat_t>									CameraRotationTransformFlowIn();
	rxcpp::observer<trans_vec_t>							CameraTranslationVectorFlowIn();
	rxcpp::observable<BufferCPU<uchar4>>					FrameBufferFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class ViewportTracingModel
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_MODELS_VIEWPORTTRACINGMODEL_H__