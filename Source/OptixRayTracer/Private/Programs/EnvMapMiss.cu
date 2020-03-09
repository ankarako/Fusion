#include <optix_world.h>
#include <RayPayload.cuh>
#include <Helpers.cuh>

rtDeclareVariable(fu::rt::PerRayData_Radiance, prd_radiance, rtPayload, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(uint2, launch_index, rtLaunchIndex, );
///=======================================
///	Miss program for 360 environment maps
///=======================================
rtTextureSampler<uchar4, 2> EnvMapTexSampler;
RT_PROGRAM void EnvMapMiss()
{
	unsigned int idx = launch_index.x + launch_index.y;
	if (idx == 0)
	{
		printf("current launch idx: %u\n", idx);
	}
	float theta = atan2f(ray.direction.x, ray.direction.z);
	float phi	= M_PIf * 0.5f - acosf(ray.direction.y);
	float u		= (theta + M_PIf) * (0.5f * M_1_PIf);
	float v		= 0.5f * (1.0f + sinf(phi));
	
	prd_radiance.Result = tex2D(EnvMapTexSampler, u, v);
}