#include <optix_world.h>
#include <RayPayload.cuh>
#include <Helpers.cuh>


rtDeclareVariable(optix::float3, eye, , );
rtDeclareVariable(optix::float3, U, , );
rtDeclareVariable(optix::float3, V, , );
rtDeclareVariable(optix::float3, W, , );

rtDeclareVariable(float, scene_epsilon, , );
rtDeclareVariable(rtObject, top_object, , );
rtDeclareVariable(rtObject, top_shadower, ,);
rtDeclareVariable(optix::uint2, launch_index, rtLaunchIndex, );
rtDeclareVariable(optix::uint2, launch_dims, rtLaunchDim, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(fu::rt::PerRayData_Radiance, prd_radiance, rtPayload, );

rtBuffer<uchar4, 2> output_buffer;

RT_PROGRAM void PinholeRaygen()
{
	optix::float2 d = make_float2(launch_index) / make_float2(launch_dims) * 2.0f - 1.0f;
	optix::float3 ray_origin = eye;
	optix::float3 ray_direction = optix::normalize(d.x * U + d.y * V + W);
	optix::Ray ray = optix::make_Ray(ray_origin, ray_direction, RadianceRayType, scene_epsilon, RT_DEFAULT_MAX);
	fu::rt::PerRayData_Radiance prd;
	prd.Importance = 1.0f;
	prd.Depth = 0;
	rtTrace(top_object, ray, prd);
	output_buffer[launch_index] = prd.Result;
}