#ifndef	__FUSION_PUBLIC_MODELS_VIDEOTRACINGMODEL_H__
#define __FUSION_PUBLIC_MODELS_VIDEOTRACINGMODEL_H__

#include <Initializable.h>
#include <Destroyable.h>
#include <Buffer.h>
#include <Components/TriangleMeshComp.h>
#include <Components/ContextComp.h>
#include <MeshData.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>
#include <GL/gl3w.h>
#include <array>

namespace fu {
namespace fusion {
///	\class VideoTracingModel
///	\brief 3D ray tracing context for rendering 360 videos (or just videos)
class VideoTracingModel	: public app::Initializable, public app::Destroyable
{
public:
	///	\typedef input_frame_t
	///	\brief the input frame type
	using input_frame_t = BufferCPU<uchar4>;
	using output_frame_t = BufferCPU<uchar4>;
	using vec_t = std::array<float, 3>;
	///	Construction
	VideoTracingModel();
	///	\brief initialize the model
	void Init() override;
	///	\brief Destroy the model
	void Destroy() override;
	/// \brief frame size input
	rxcpp::observer<uint2>				FrameSizeFlowIn();
	/// \brief rendered buffer output
	rxcpp::observable<output_frame_t>	FrameFlowOut();
	///	\brief frame input
	rxcpp::observer<input_frame_t>	FrameFlowIn();
	rxcpp::observer<io::MeshData>	MeshDataFlowIn();
	rxcpp::observer<vec_t>			PerfcapTranslationFlowIn();
	rxcpp::observer<vec_t>			PerfcapRotationFlowIn();
	rxcpp::observer<float>			PerfcapScaleFlowIn();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class VideoTracingModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_VIDEOTRACINGMODEL_H__