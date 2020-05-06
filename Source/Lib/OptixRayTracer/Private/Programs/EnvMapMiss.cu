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
	//unsigned int idx = launch_index.x + launch_index.y;
	//float theta = atan2f(ray.direction.x, ray.direction.z);
	//float phi	= M_PIf * 0.5f - acosf(ray.direction.y);
	//float u		= (theta + M_PIf) * (0.5f * M_1_PIf);
	//float v		= 0.5f * (1.0f + sinf(phi));
	float u = static_cast<float>(launch_index.x) / static_cast<float>(launch_dims.x);
	float v = static_cast<float>(launch_index.y) / static_cast<float>(launch_dims.y);
	float4 color = tex2D(EnvMapTexSampler, u, v);
	// if (idx == 0)
	// 	printf("color: (%f, %f, %f, %f)\n", color.x, color.y, color.x, color.w);
	prd_radiance.Result = fu::rt::make_color(optix::make_float3(color));
	// if (idx == 0)
	// 	printf("color: (%u, %u, %u, %u)\n", prd_radiance.Result.x, prd_radiance.Result.y, prd_radiance.Result.x, prd_radiance.Result.w);
}