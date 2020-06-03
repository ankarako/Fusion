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
	optix::Matrix4x4		Intrinsics;
	optix::float3			Eye{ 0.0f, 0.0f, 0.0f };
	optix::float3			Up{ 0.0f, -1.0f, 0.0f };
	optix::float3			LookAt{ 0.0f, 0.0f, 1.0f };
	optix::float3			Left{ 0.0f, 0.0f, 0.0f };
	optix::float2			FOV{ 90, 59 };
	int						CamPlaneWidth{ 0 };
	int						CamPlaneHeight{ 0 };
	float					AspectRatio{ 0.0f };
	optix::Buffer			TextureBuffer;
	optix::TextureSampler	TextureSampler;
	optix::Program			RaygenProg;
	optix::Buffer			TexturingOutputBuffer;
	optix::Matrix4x4		Transform;
	optix::Buffer			OutputColorBuffer;
	optix::Buffer			OutputTexcoordBuffer;
	optix::Buffer			OutputWeightBuffer;
	optix::Buffer			OutputCamIdBuffer;
	optix::Buffer			DebugBuffer;
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