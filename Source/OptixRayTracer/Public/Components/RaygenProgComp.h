#ifndef __OPTIXRAYTRACER_PUBLIC_COMPONENTS_RAYGENPROGCOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_RAYGENPROGCOMP_H__

#include <optix.h>
#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_vector_types.h>
#include <optixu/optixu_vector_functions.h>
#include <memory>

namespace fu {
namespace rt {
///	\struct OptixPinholeRaygenProgComp
///	\brief an optix program component for pinhole ray generation
struct RaygenProgCompObj
{
	optix::Program	RaygenProg;
	optix::float3	Eye{ 0.0f, 0.0f, 0.0f };
	optix::float3	Up{ 0.0f, 0.0f, 0.0f };
	optix::float3	Lookat{ 0.0f, 0.0f, 0.0f };
	int		ViewWidth{ 0 };
	int		ViewHeight{ 0 };
	float	AspectRatio{ 0.0f };
	optix::Buffer	OutputBuffer;
};	///	!OptixPinholeRaygenProgCompObj
///	\typedef OptixPinholeRaygenProgComp
///	\brief a reference counted OptixPinholeRaygenProgCompObj
using RaygenProgComp = std::shared_ptr<RaygenProgCompObj>;
///	\brief create an OptixPinholeRaygenProgComp
static RaygenProgComp CreateRaygenProgComponent()
{
	return std::make_shared<RaygenProgCompObj>();
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_RAYGENPROGCOMP_H__