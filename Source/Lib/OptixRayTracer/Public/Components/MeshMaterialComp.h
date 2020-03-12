#ifndef __OPTIXRAYTRACER_PUBLIC_COMPONENTS_MESHMATERIALCOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_MESHMATERIALCOMP_H__

#include <optix.h>
#include <optixu/optixpp_namespace.h>
#include <memory>

namespace fu {
namespace rt {
///	\struct MeshMaterialCompObj
///	\brief holds optix mesh material data
struct MeshMaterialCompObj
{
	optix::Material Material;
	optix::Program	ClosestHitProg;
	optix::Program	AnyHitProg;
};	///	!struct MeshMaterialCompObj
///	\typedef MeshMaterialComp
///	\brief a reference counted MeshMaterialComp
using MeshMaterialComp = std::shared_ptr<MeshMaterialCompObj>;
///	\brief create a MeshMaterialComp
static MeshMaterialComp CreateMeshMaterialComp()
{
	return std::make_shared<MeshMaterialCompObj>();
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_MESHMATERIALCOMP_H__