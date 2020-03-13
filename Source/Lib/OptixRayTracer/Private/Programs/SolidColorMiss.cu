#include <optix_world.h>
#include <RayPayload.cuh>
#include <Helpers.cuh>

rtDeclareVariable(fu::rt::PerRayData_Radiance, prd_radiance, rtPayload, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(float3, solid_color, , );
///==============================================
/// Simple miss program
/// puts a specified solid color when ray misses
///==============================================
RT_PROGRAM void SolidColorMiss()
{
	prd_radiance.Result = fu::rt::make_color(solid_color);
}