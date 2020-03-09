#ifndef __OPTIXRAYTRACER_PUBLIC_COMPONENTS_TRIANGLEMESHCOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_TRIANGLEMESHCOMP_H__

#include <optix.h>
#include <optixu/optixpp_namespace.h>
#include <memory>

namespace fu {
namespace rt {
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
	optix::Program			IntersectionProg;
	optix::Program			BoundingBoxProgram;
	optix::Program			ClosestHitProgram;
	optix::Program			AnyHitProgram;
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