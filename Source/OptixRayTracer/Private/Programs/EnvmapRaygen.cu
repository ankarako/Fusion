#include <optix_world.h>
#include <Helpers.cuh>
#include <RayPayload.cuh>

rtDeclareVariable(float3, eye, , );
rtDeclareVariable(float3, U, , );
rtDeclareVariable(float3, V, , );
rtDeclareVariable(float3, W, , );
rtDeclareVariable(float,  scene_epsilon, , );

rtDeclareVariable(rtObject, top_object, , );
rtDeclareVariable(unsigned int, radiance_ray_type, , );
rtDeclareVariable(uint2, launch_index, rtLaunchIndex, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );

rtBuffer<uchar4, 2> output_buffer;

RT_PROGRAM void EnvMapRaygen()
{
	optix::size_t2 screen_size = output_buffer.size();
	optix::float2 d = optix::make_float2(launch_index) / optix::make_float2(screen_size) * optix::make_float2(2.0f * M_PIf, M_PIf) + optix::make_float2(M_PIf, 0);
	optix::float3 angle = optix::make_float3(cos(d.x) * sin(d.y), -cos(d.y), sin(d.y) * sin(d.y));
	optix::float3 ray_origin = eye;
	optix::float3 ray_direction = optix::normalize(angle.x * optix::normalize(U) + angle.y * optix::normalize(V) + angle.z * optix::normalize(W));
	optix::Ray ray = optix::make_Ray(ray_origin, ray_direction, radiance_ray_type, scene_epsilon, RT_DEFAULT_MAX);

	fu::rt::PerRayData_Radiance prd;
	prd.Importance = 1.0f;
	prd.Depth = 0;

	output_buffer[launch_index] = prd.Result;
}