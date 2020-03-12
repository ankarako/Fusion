#ifndef __FUSION_PUBLIC_MODELS_RAYTRACINGMODEL_H__
#define __FUSION_PUBLIC_MODELS_RAYTRACINGMODEL_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace fusion {
///	\class RayTracingModel
///	\brief 3D ray tracing
class RayTracingModel : public app::Initializable
{
public:
	/// Construction
	RayTracingModel();
	/// \brief initialize the model
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class RayTracingModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_RAYTRACINGMODEL_H__