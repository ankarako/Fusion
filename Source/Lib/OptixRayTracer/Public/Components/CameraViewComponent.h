#ifndef __OPTIXRAYTRACER_PUBLIC_COMPONENTS_CAMERAVIEWCOMPONENT_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_CAMERAVIEWCOMPONENT_H__

#include <optix_world.h>
#include <optixu/optixpp_namespace.h>
#include <memory>

namespace fu {
namespace rt {
static constexpr optix::float3 k_BBoxMin{ 1.0e16, 1.0e16 , 1.0e16 };
static constexpr optix::float3 k_BBoxMax{ -1.0e16, -1.0e16 , -1.0e16 };

struct CameraViewportCompObj
{
	int 					ViewportHeight;
	int						ViewportWidth;
	optix::Buffer			TextureBuffer;
	optix::TextureSampler	TextureSampler;
	optix::Program			IntersectionProgram;
	optix::Program			BoundingBoxProgram;
	optix::Program			ClosestHitProgram;
	optix::Program			AnyHitProgram;
	optix::Geometry			Geometry;
	optix::Material			Material;
	optix::GeometryInstance	GInstance;
	optix::GeometryGroup 	GGroup;
	optix::Aabb				Bbox{ k_BBoxMin, k_BBoxMax };
	optix::Acceleration		Acceleration;
	optix::Transform		Transform;
	optix::Matrix4x4		TransMat;
};

using CameraViewportComp = std::shared_ptr<CameraViewportCompObj>;

static CameraViewportComp CreateCameraViewportComponent()
{
	return std::make_shared<CameraViewportCompObj>();
}
}	///	!namespace rt
}	///	!namespace view
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_CAMERAVIEWCOMPONENT_H__