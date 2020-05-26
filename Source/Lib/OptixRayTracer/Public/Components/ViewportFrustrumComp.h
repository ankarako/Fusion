#ifndef __OPTIXRAYTRACER_PUBLIC_COMPONENTS_PARALLELOGRAMCOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_PARALLELOGRAMCOMP_H__

#include <optix_world.h>
#include <memory>

namespace fu {
namespace rt {
///	\struct ViewportFrustrumCompObj
///	\brief a ViewportFrustrumCompObj componret
struct ViewportFrustrumCompObj
{
	optix::Geometry			Geometry;
	optix::Material 		Material;
	optix::GeometryInstance	GInstance;
	optix::GeometryGroup	GGroup;
	optix::Transform		Transform;
	optix::Acceleration		Acceleration;
	optix::Matrix4x4		TransMat;
	optix::float4			Plane;
	optix::float3 			V1;
	optix::float3			V2;
	optix::float3			Anchor;
	optix::Buffer			TextureBuffer;
	optix::TextureSampler	TextureSampler;
	optix::Program			IntersectionProgram;
	optix::Program			BoundingBoxProgram;
	optix::Program			ClosestHitProgram;
	optix::Program			AnyhitProgram;

};	///	!struct ViewportFrustrumCompObj
///	\typedef ViewportFrustrumComp
///	\brief a shared viewport frustrum component
using ViewportFrustrumComp = std::shared_ptr<ViewportFrustrumCompObj>;
///	\brief create a ViewportFrustrumComp
static ViewportFrustrumComp CreateViewportFrustrumComponent()
{
	return std::make_shared<ViewportFrustrumCompObj>();
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_PARALLELOGRAMCOMP_H__