#include <optix_world.h>
#include <Helpers.cuh>
#include <RayPayload.cuh>


rtDeclareVariable(rtObject, top_object, , );
rtDeclareVariable(float, scene_epsilon, , );

rtDeclareVariable(optix::float4, transparent_color, , );

rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(float, hit_dist, rtIntersectionDistance, );
rtDeclareVariable(fu::rt::PerRayData_Radiance, prd_radiance, rtPayload, );

RT_PROGRAM void transparent_color_closest_hit()
{
	prd_radiance.Result = fu::rt::make_color(transparent_color);
	optix::float3 hitpoint = ray.origin + hit_dist * ray.direction;
	optix::Ray continuationRay = optix::make_Ray(hitpoint, ray.direction, RadianceRayType, scene_epsilon, RT_DEFAULT_MAX);
	fu::rt::PerRayData_Radiance new_prd;
	rtTrace(top_object, continuationRay, new_prd);
	optix::uchar4 color = new_prd.Result;
	optix::float4 fcolor = 
		optix::make_float4((float)color.x, (float)color.y, (float)color.z, (float)color.w);
	fcolor /= 255.f;
	fcolor = fcolor * (1.0f - transparent_color.w);

	prd_radiance.Result = fu::rt::make_color(fcolor);
}

RT_PROGRAM void transparent_color_any_hit()
{
	//prd_radiance.Result = fu::rt::make_color(transparent_color);
}