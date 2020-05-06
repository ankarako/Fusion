#include <optix_world.h>
#include <RayPayload.cuh>
#include <Helpers.cuh>

rtDeclareVariable(fu::rt::PerRayData_Radiance, prd_radiance, rtPayload, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(uint2, launch_index, rtLaunchIndex, );
rtDeclareVariable(uint2, launch_dims, rtLaunchDim, );
///=======================================
///	Miss program for 360 environment maps
///=======================================
rtTextureSampler<float4, 2> EnvMapTexSampler;
RT_PROGRAM void EnvMapMiss()
{
	float u = static_cast<float>(launch_index.x) / static_cast<float>(launch_dims.x);
	float v = static_cast<float>(launch_index.y) / static_cast<float>(launch_dims.y);
	float4 color = tex2D(EnvMapTexSampler, u, v);
	prd_radiance.Result = fu::rt::make_color(optix::make_float3(color));
}