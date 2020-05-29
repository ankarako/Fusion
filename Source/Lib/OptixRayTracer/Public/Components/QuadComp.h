#ifndef	__OPTIXRAYTRACER_PUBLIC_COMPONENTS_QUADCOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_QUADCOMP_H__

#include <optix_world.h>
#include <memory>

namespace fu {
namespace rt {
///	\struct QuadCompObj
///	\brief a quad component
struct QuadCompObj
{
	optix::float3 			Anchor;
	float					Width;
	float					Height;
	optix::float3			Color;
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
};	///	!struct QuadCompObj
///	\typedef QuadComp
///	\brief a shared QuadCompObj object
using QuadComp = std::shared_ptr<QuadCompObj>;
///	\brief make a QuadComp object
static QuadComp CreateQuadComponent(const optix::float3& anchor, float width, float height)
{
	QuadComp comp = std::make_shared<QuadCompObj>();
	comp->Anchor = anchor;
	comp->Width = width;
	comp->Height = height;
	return comp;
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_QUADCOMP_H__