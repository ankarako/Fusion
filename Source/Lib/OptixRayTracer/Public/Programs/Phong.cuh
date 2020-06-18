#ifndef __OPTIXRAYTRACER_PUBLIC_PROGRAMS_PHONG_CUH__
#define __OPTIXRAYTRACER_PUBLIC_PROGRAMS_PHONG_CUH__

#include <optix_world.h>
#include <Helpers.cuh>
#include <BasicLight.cuh>
#include <RayPayload.cuh>

namespace fu {
namespace rt {

static __device__ void PhongShade(
	optix::float3 p_Kd, 
	optix::float3 p_Ka,
	optix::float3 p_Ks, 
	optix::float3 p_Kr,
	optix::float3 ambient_color,
	BasicLight* lights,
	int num_lights,
	float p_phong_exp, 
	optix::float3 p_normal,
	float hitDist, 
	optix::Ray currentRay)
{
	optix::float3 hit_point = currentRay.origin + hitDist * currentRay.direction;

	optix::float3 result = p_Ka * ambient_color;

	for (int i = 0; i < num_lights; ++i)
	{
		BasicLight light = lights[i];
		float lDist = optix::length(light.pos - hit_point);
		optix::float3 L = optix::normalize(light.pos - hit_point);
		float nDl = optix::dot(p_normal, L);
		optix::float3 light_attenuation = optix::make_float3(static_cast<float>(nDl > 0.0f));

		if (nDl > 0.0f && light.CastsShadow)
		{
			PerRayData_Shadow prd_shadow;
			prd_shadow.Attenuation = optix::make_float3(1.0f);
		}

	}
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_PROGRAMS_PHONG_CUH__