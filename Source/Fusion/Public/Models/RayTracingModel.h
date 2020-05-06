#ifndef __FUSION_PUBLIC_MODELS_RAYTRACINGMODEL_H__
#define __FUSION_PUBLIC_MODELS_RAYTRACINGMODEL_H__

#include <Initializable.h>
#include <Updateable.h>
#include <Destroyable.h>
#include <Buffer.h>
#include <Components/ContextComp.h>
#include <Components/TriangleMeshComp.h>
#include <Components/PointCloudComp.h>
#include <MeshData.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>
#include <string>
#include <array>

namespace fu {
namespace fusion {
///	\class RayTracingModel
///	\brief 3D ray tracing
class RayTracingModel : public app::Initializable, public app::Updateable, public app::Destroyable
{
public:
	using mat_t = std::array<float, 16>;
	using vec_t = std::array<float, 3>;
	/// Construction
	RayTracingModel();
	/// \brief initialize the model
	void Init() override;
	///	\brief update the model
	void Update() override;
	///	\brief destroy the model
	void Destroy() override;
	///
	void SetIsValid(bool val);
	bool GetIsValid() const;
	rt::ContextComp& GetCtxComp();
	///
	rxcpp::observer<uint2>			LaunchSizeFlowIn();
	rxcpp::observer<void*>			OnLaunch();
	rxcpp::observer<mat_t>			CameraRotationTransformFlowIn();
	rxcpp::observer<vec_t>			CameraTranslationFlowIn();
	rxcpp::observer<float>			CullingPlanePositionFlowIn();
	rxcpp::observer<float>			PointcloudPointSizeFlowIn();
	rxcpp::observer<io::MeshData>	MeshDataFlowIn();
	rxcpp::observer<vec_t>			PerfcapTranslationFlowIn();
	rxcpp::observer<vec_t>			PerfcapRotationFlowIn();
	rxcpp::observer<float>			PerfcapScaleFlowIn();
	///
	rxcpp::observable<BufferCPU<uchar4>> FrameFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class RayTracingModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_RAYTRACINGMODEL_H__