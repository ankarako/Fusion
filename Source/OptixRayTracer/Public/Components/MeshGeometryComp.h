#ifndef __OPTIXRAYTRACER_PUBLIC_COMPONENTS_MESHGEOMETRYCOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_MESHGEOMETRYCOMP_H__

#include <optix.h>
#include <optixu/optixpp_namespace.h>
#include <memory>

namespace fu {
namespace rt {
///	\struct MeshGeometryCompObj
///	\brief mesh geoemtry component
struct MeshGeometryCompObj
{
	optix::Geometry	Geometry;
	optix::Buffer	VertexBuffer;
	optix::Buffer	NormalBuffer;
	optix::Buffer	TIndexBuffer;
	optix::Program	IntersectionProg;
};	///	!struct MeshGeometryComp
///	\typedef MeshGeometryComp
///	\brief a reference counted MeshGeometryCompObj
using MeshGeometryComp = std::shared_ptr<MeshGeometryCompObj>;
///	\brief create a MeshGeometryComp
static MeshGeometryComp CreateMeshGeometryComp()
{
	return std::make_shared<MeshGeometryCompObj>();
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_MESHGEOMETRYCOMP_H__