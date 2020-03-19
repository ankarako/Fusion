#include <optix_world.h>
#include <Helpers.cuh>
#include <RayPayload.cuh>

rtDeclareVariable(optix::float3, solid_color, , );
rtDeclareVariable(fu::rt::PerRayData_Radiance, prd_radiance, rtPayload, );

RT_PROGRAM void solid_color_closest_hit()
{
	prd_radiance.Result = fu::rt::make_color(solid_color);
}

RT_PROGRAM void solid_color_any_hit()
{
	prd_radiance.Result = fu::rt::make_color(solid_color);
}