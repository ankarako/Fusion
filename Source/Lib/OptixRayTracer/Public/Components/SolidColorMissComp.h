#ifndef	__OPTIXRAYTRACER_PUBLIC_COMPONENTS_SOLIDCOLORMISSCOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_SOLIDCOLORMISSCOMP_H__

#include <optix_world.h>
#include <memory>

namespace fu {
namespace rt {
///	\struct SolidColorMissCompObj
///	\brief holds solid color miss program related data
struct SolidColorMissCompObj
{
	optix::float3 	SolidColor;
	optix::Program	MissProg;
	/// Construction
	SolidColorMissCompObj(optix::float3 color)
		: SolidColor(color)
	{ }
};	///	!struct SolidColorMissCompObj
///	\typedef SolidColorMissComp
///	\brief a reference counted solicolor miss comp obj
using SolidColorMissComp = std::shared_ptr<SolidColorMissCompObj>;
///	\brief create a silid color miss comp with the specified solid color
static SolidColorMissComp CreateSolidColorMissComponent(optix::float3 color)
{
	return std::make_shared<SolidColorMissCompObj>(color);
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_SOLIDCOLORMISSCOMP_H__