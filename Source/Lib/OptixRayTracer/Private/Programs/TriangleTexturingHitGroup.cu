#include <optix_world.h>
#include <Helpers.cuh>
#include <RayPayload.cuh>

rtDeclareVariable(optix::float2, 				cur_texcoord, 		attribute VertexTexcoord, );
rtDeclareVariable(optix::float3, 				cur_geom_normal,	attribute GeometricNormal, );
rtDeclareVariable(optix::float3, 				cur_shad_normal,	attribute ShadingNormal, );
rtDeclareVariable(optix::uchar4, 				cur_color,			attribute TriangleColor, );
rtDeclareVariable(fu::rt::PerRayData_Texturing,	prd_texturing, 		rtPayload, );
rtDeclareVariable(optix::Ray,					ray,				rtCurrenRay, );

RT_PROGRAM void closest_hit_texturing()
{
	optix::float3 world_shading_normal = optix::normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, cur_shad_normal));
	optix::float3 world_geometric_normal = optix::normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, cur_shad_normal));

	optix::float3 ffnormal = optix::faceforward(world_shading_normal, -ray.direction, world_geometric_normal);
	
	prd_texturing.Output = { prd_texturing.PixelValue, cur_texcoord, prd_texturing.CameraId, 1.0f };
}

RT_PROGRAM void any_hit()
{
	
}