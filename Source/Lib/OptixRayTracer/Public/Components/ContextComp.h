#ifndef	__OPTIXRAYTRACER_PUBLIC_COMPONENTS_CONTEXTCOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_CONTEXTCOMP_H__

#include <optix.h>
#include <optixu/optixpp_namespace.h>
#include <memory>

namespace fu {
namespace rt {
///	\struct OptixContextComp
///	\brief holds an optix context
struct ContextCompObj
{
	optix::Context Context;
};	///	!struct OptixContextComp
///	\typedef OptixContextComp
///	\brief a reference counted OptixContextCompObj
using ContextComp = std::shared_ptr<ContextCompObj>;
///	\brief create an OptixContextComp
static ContextComp CreateContextComponent()
{
	return std::make_shared<ContextCompObj>();
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_CONTEXTCOMP_H__