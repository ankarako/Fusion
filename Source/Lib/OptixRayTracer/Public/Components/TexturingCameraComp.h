#ifndef __OPTIXRAYTRACER_PUBLIC_COMPONENTS_TEXTURINGCAMERACOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_TEXTURINGCAMERACOMP_H__

#include <optix_world.h>
#include <memory>

namespace fu {
namespace rt {
struct TexturingCameraCompObj
{
	int						Id{ 0 };
	optix::Matrix4x4		Extrinsics;
	optix::Matrix3x3		Intrinsics;
	optix::float3			Eye{ 0.0f, 0.0f, 0.0f };
	optix::float3			Up{ 0.0f, 0.0f, 0.0f };
	optix::float3			LookAt{ 0.0f, 0.0f, 0.0f };
	optix::float3			Left{ 0.0f, 0.0f, 0.0f };
	int						CamPlaneWidth{ 0 };
	int						CamPlaneHeight{ 0 };
	float					AspectRatio{ 0.0f };
	optix::Buffer			TextureBuffer;
	optix::TextureSampler	TextureSampler;
	optix::Program			RaygenProg;
};	///	!struct TexturingCameraCompObj
///	\typedef TexturingCameraComp
///	\brief a shared TexturingCameraCompObj
using TexturingCameraComp = std::shared_ptr<TexturingCameraCompObj>;
///	\brief create a TexturingCameraComp object
static TexturingCameraComp CreateTexturingCameraComponent()
{
	return std::make_shared<TexturingCameraCompObj>();
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_TEXTURINGCAMERACOMP_H__