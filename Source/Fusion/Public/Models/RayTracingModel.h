#ifndef __FUSION_PUBLIC_MODELS_RAYTRACINGMODEL_H__
#define __FUSION_PUBLIC_MODELS_RAYTRACINGMODEL_H__

#include <Initializable.h>
#include <Updateable.h>
#include <Destroyable.h>
#include <Buffer.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>
#include <string>

namespace fu {
namespace fusion {
///	\class RayTracingModel
///	\brief 3D ray tracing
class RayTracingModel : public app::Initializable, public app::Updateable, public app::Destroyable
{
public:
	/// Construction
	RayTracingModel();
	/// \brief initialize the model
	void Init() override;
	///	\brief update the model
	void Update() override;
	///	\brief destroy the model
	void Destroy() override;
	///	\brief load a 3D asset
	///	\param filepath the path to the file to load
	void LoadAsset(const std::string& filepath);
	///
	rxcpp::observer<uint2>  LaunchSizeFlowIn();
	///
	rxcpp::observable<BufferCPU<uchar4>> FrameFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class RayTracingModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_RAYTRACINGMODEL_H__