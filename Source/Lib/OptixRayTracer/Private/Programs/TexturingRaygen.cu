#include <optix_world.h>
#include <Helpers.cuh>
#include <RayPayload.cuh>

rtBuffer<uchar4>						TextureBuffer;
rtBuffer<optix::uchar4> 				OutputBuffer;
rtTextureSampler<uchar4, 2>				TextureSampler;

rtDeclareVariable(optix::float3, eye, , );
rtDeclareVariable(optix::float3, up, , );
rtDeclareVariable(optix::float3, left, , );
rtDeclareVariable(optix::float3, lookat, , );
rtDeclareVariable(int, camId, , );
rtDeclareVariable(float, scene_epsilon, , );
rtDeclareVariable(rtObject, top_object, , );
rtDeclareVariable(optix::uint2, launch_idx, rtLaunchIndex, );
rtDeclareVariable(optix::uint2, launch_dims, rtLaunchDim, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(fu::rt::PerRayData_Texturing, prd_texturing, rtPayload, );
rtDeclareVariable(optix::Matrix4x4, extrinsics, , );
rtDeclareVariable(optix::Matrix3x3, intrinsics, , );

RT_PROGRAM void TexturingRaygen()
{
	float u = (float)launch_idx.x / (float)launch_dims.x;
	float v = (float)launch_idx.y / (float)launch_dims.y;
	uchar4 color = tex2D(TextureSampler, u, v);
	fu::rt::PerRayData_Texturing prd_texturing;
	prd_texturing.PixelValue = color;
	prd_texturing.CameraId = camId;
	optix::float3 ray_origin = eye;
	optix::float3 ray_direction = optix::normalize(left * u + up * v + lookat);
	optix::Ray ray = optix::make_Ray(ray_origin, ray_direction, 0, scene_epsilon, RT_DEFAULT_MAX);
	rtTrace(top_object, ray, prd_texturing);
	
}