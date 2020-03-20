#ifndef	__OPTIXRAYTRACER_PUBLIC_COMPONENTS_BASICLIGHTCOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_BASICLIGHTCOMP_H__

#include <optix_world.h>
#include <memory>

namespace fu {
namespace rt {

struct BasicLightCompObj
{
	optix::float3 	Position;
	optix::float3 	Color;
	int	 			CastsShadow;
	int 			Padding;
};	///	!struct BasicLightObj
///	\typedef BasicLightComp
///	\brief a reference counted BasicLightCompObj
using BasicLightComp = std::shared_ptr<BasicLightCompObj>;
///	\brief create a basic light componrnt
static BasicLightComp CreateBasicLightComponent()
{
	return std::make_shared<BasicLightCompObj>();
}
}	///	!namespace rt
}	///	!namespce fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_BASICLIGHTCOMP_H__