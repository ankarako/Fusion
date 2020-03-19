#ifndef __OPTIXRAYTRACER_PUBLIC_COMPONENTS_TRIANGLEMESHCOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_TRIANGLEMESHCOMP_H__

#include <optix_world.h>
#include <optixu/optixpp_namespace.h>
#include <memory>

namespace fu {
namespace rt {
	static constexpr optix::float3 k_BBoxMin{ 1.0e16, 1.0e16 , 1.0e16 };
	static constexpr optix::float3 k_BBoxMax{ -1.0e16, -1.0e16 , -1.0e16 };
///	\struct MeshGeometryCompObj
///	\brief mesh geoemtry component
struct TriangleMeshCompObj
{
	optix::Geometry			Geometry;
	optix::Material 		Material;
	optix::GeometryInstance	GInstance;
	optix::Buffer			VertexBuffer;
	optix::Buffer			NormalBuffer;
	optix::Buffer			TIndexBuffer;
	optix::Buffer			TexCoordBuffer;
	optix::Buffer			MaterialBuffer;
	optix::Buffer			ColorsBuffer;
	optix::Program			IntersectionProg;
	optix::Program			BoundingBoxProgram;
	optix::Program			ClosestHitProgram;
	optix::Program			AnyHitProgram;
	optix::Aabb				BBox{ k_BBoxMin, k_BBoxMax };
	optix::Transform		Transform;
	optix::Acceleration		Acceleration;
	optix::GeometryGroup	GGroup;
	
};	///	!struct MeshGeometryComp
///	\typedef MeshGeometryComp
///	\brief a reference counted MeshGeometryCompObj
using TriangleMeshComp = std::shared_ptr<TriangleMeshCompObj>;
///	\brief create a MeshGeometryComp
static TriangleMeshComp CreateTriangleMeshComponent()
{
	return std::make_shared<TriangleMeshCompObj>();
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_TRIANGLEMESHCOMP_H__