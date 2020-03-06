#ifndef	__OPTIXRAYTRACER_PUBLIC_COMPONENTS_MESHINSTANCECOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_MESHINSTANCECOMP_H__

#include <optix.h>
#include <optixu/optixpp_namespace.h>
#include <memory>

namespace fu {
namespace rt {
///	\struct MeshInstanceCompObj
///	\brief holds optix group and acceleration structure
struct MeshInstanceCompObj
{
	optix::Transform		Transform;
	optix::Acceleration		Acceleration;
	optix::GeometryGroup	GGroup;	
	optix::GeometryInstance GInstance;
};	///	!struct MeshInstanceCompObj
///	\typedef MeshInstanceComp
///	\brief a reference counted MeshInstanceCompObj
using MeshInstanceComp = std::shared_ptr<MeshInstanceCompObj>;
///	\brief create a MeshInstanceComp
static MeshInstanceComp CreateMeshInstanceComponent()
{
	return std::make_shared<MeshInstanceCompObj>();
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_MESHINSTANCECOMP_H__