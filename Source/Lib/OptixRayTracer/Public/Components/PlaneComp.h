#ifndef __OPTIXRAYTRACER_PUBLIC_COMPONENTS_PLANECOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_PLANECOMP_H__

#include <optix_world.h>
#include <memory>

namespace fu {
namespace rt {

struct PlaneCompObj
{
	optix::float3 			Anchor;
	float					Width;
	float					Height;
	optix::float4			Color;
	optix::Geometry			Geometry;
	optix::Material			Material;
	optix::GeometryInstance GInstance;
	optix::GeometryGroup	GGroup;
	optix::Acceleration		Acceleration;
	optix::Transform		Transform;
	optix::Matrix4x4		TransMat;
	optix::Buffer			VertexBuffer;
	optix::Buffer			TextureBuffer;
	optix::TextureSampler	TextureSampler;
	optix::Program			IntersectionProgram;
	optix::Program			BoundingBoxProgram;
	optix::Program			ClosestHitProgram;
	optix::Program			AnyHitProgram;
};	///	!struct PlaneCompObj

using PlaneComp = std::shared_ptr<PlaneCompObj>;

static PlaneComp CreatePlaneComponent(const optix::float4& color, const optix::float3& anchor, float width, float height)
{
	PlaneComp res = std::make_shared<PlaneCompObj>();
	res->Anchor = anchor;
	res->Width = width;
	res->Height = height;
	res->Color = color;
	return res;
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_PLANECOMP_H__