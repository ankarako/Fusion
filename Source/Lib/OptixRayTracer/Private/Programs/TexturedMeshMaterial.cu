#include <optix_world.h>
#include <Helpers.cuh>
#include <RayPayload.cuh>

rtBuffer<uchar4>			TextureBuffer;
rtTextureSampler<float4, 2> TextureSampler;

rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(optix::float2, cur_texcoord, 		attribute VertexTexcoord, );
rtDeclareVariable(optix::uchar4, cur_color,			attribute TriangleColor, );
rtDeclareVariable(optix::float3, cur_geom_normal,	attribute GeometricNormal, );
rtDeclareVariable(optix::float3, cur_shad_normal,	attribute ShadingNormal, );
rtDeclareVariable(optix::float3, back_hit_point,	attribute BackHitPoint, );
rtDeclareVariable(optix::float3, front_hit_point,	attribute FrontHitPoint, );
rtDeclareVariable(fu::rt::PerRayData_Radiance,	prd_radiance, 		rtPayload, );

RT_PROGRAM void closest_hit_radiance()
{
	float u = cur_texcoord.x;
	float v = cur_texcoord.y;
	float4 color = tex2D(TextureSampler, u, v);
	prd_radiance.Result = fu::rt::make_color(optix::make_float3(color));
}

RT_PROGRAM void any_hit()
{

}