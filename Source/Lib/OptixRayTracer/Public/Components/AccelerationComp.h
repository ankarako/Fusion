#ifndef	__OPTIXRAYTRACER_PUBLIC_COMPONENTS_ACCELERATIONCOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_ACCELERATIONCOMP_H__

#include <optix.h>
#include <optixu/optixpp_namespace.h>
#include <memory>

namespace fu {
namespace rt {
///	\struct MeshInstanceCompObj
///	\brief holds optix group and acceleration structure
struct AccelerationCompObj
{
	optix::Transform		Transform;
	optix::Acceleration		Acceleration;
	optix::Group			Group;	
};	///	!struct MeshInstanceCompObj
///	\typedef MeshInstanceComp
///	\brief a reference counted MeshInstanceCompObj
using AccelerationComp = std::shared_ptr<AccelerationCompObj>;
///	\brief create a MeshInstanceComp
static AccelerationComp CreateAccelerationComponent()
{
	return std::make_shared<AccelerationCompObj>();
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_ACCELERATIONCOMP_H__