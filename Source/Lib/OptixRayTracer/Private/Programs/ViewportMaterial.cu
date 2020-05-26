#include <optix_world.h>
#include <RayPayload.cuh>
#include <Helpers.cuh>

rtDeclareVariable(fu::rt::PerRayData_Radiance, prd_radiance, rtPayload, );
rtDeclareVariable(optix::float3, texcoord, attribute texcoord, );

rtTextureSampler<float4, 2> ViewportTextureSampler;
RT_PROGRAM void viewport_closest_hit()
{
	const float u = texcoord.x;
	const float v = texcoord.y;
	const float4 color = tex2D(ViewportTextureSampler, u, v);
	prd_radiance.Result = fu::rt::make_color(optix::make_float3(color));
}
