#include <optix_world.h>
#include <Helpers.cuh>
#include <RayPayload.cuh>

rtBuffer<optix::uchar4>					TextureBuffer;
rtTextureSampler<float4, 2>				TextureSampler;

rtBuffer<optix::uchar4, 2>				OutputColorBuffer;
rtBuffer<optix::float2, 2>				OutputTexcoordBuffer;
// // TODO: delete the debug buffer
// rtBuffer<optix::uchar4, 2>				DebugBuffer;
rtBuffer<float, 2>						OutputWeightBuffer;
rtBuffer<int, 2>						OutputCamIdBuffer;

rtDeclareVariable(int, camId, , );
rtDeclareVariable(unsigned int, dim_mult, , );
rtDeclareVariable(float, scene_epsilon, , );
rtDeclareVariable(rtObject, top_object, , );
rtDeclareVariable(optix::uint2, launch_idx, rtLaunchIndex, );
rtDeclareVariable(optix::uint2, launch_dims, rtLaunchDim, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(fu::rt::PerRayData_Texturing, prd_texturing, rtPayload, );
rtDeclareVariable(optix::Matrix4x4, extrinsics, , );
rtDeclareVariable(optix::Matrix4x4, intrinsics, , );

RT_PROGRAM void TexturingRaygen()
{
	const unsigned int tu = launch_idx.x / dim_mult;
	const unsigned int tv = launch_idx.y / dim_mult;
	// rtPrintf("launch dims: %u x %u\n", launch_dims.x, launch_dims.y);
	// rtPrintf("lidx.x x lidx.y | tu x tv: %u x %u | %u x %u\n", launch_idx.x, launch_idx.y, tu, tv);
	float4 color = tex2D(TextureSampler, tu, tv);
	uchar4 pix = fu::rt::make_color(optix::make_float3(color));

	fu::rt::PerRayData_Texturing prd_texturing;
	prd_texturing.PixelValue = pix;
	prd_texturing.Output = { optix::make_uchar4(0, 0, 0, 0), optix::make_float2(0, 0), 0.0f, camId };
	
	const optix::float4 eye = extrinsics * optix::make_float4(0.0f, 0.0f, 0.0f, 1.0f);
	const float fx = intrinsics.getRow(0).x;
	const float fy = intrinsics.getRow(1).y;
	const float cx = intrinsics.getRow(0).z;
	const float cy = intrinsics.getRow(1).z;

	const float inv_mult = 1.0f / (float)dim_mult;
	const float u = (float)launch_idx.x * inv_mult;
	const float v = (float)launch_idx.y * inv_mult;
	const float x = (u - cx) * 1.0f / fx;
	const float y = (v - cy) * 1.0f / fy;

	const optix::float3 ddir = optix::make_float3(x, y, 1.0f);
	// optix::float4 ss = (intrinsics. * optix::make_float4(ddir, 0.0f));
	const optix::float4 dir = extrinsics * optix::make_float4(optix::normalize(ddir), 0.0f);
	const optix::float3 ray_origin = optix::make_float3(eye);
	const optix::float3 ray_direction = optix::normalize(optix::make_float3(dir));
	
	optix::Ray ray = optix::make_Ray(ray_origin, ray_direction, 0, scene_epsilon, RT_DEFAULT_MAX);
	rtTrace(top_object, ray, prd_texturing);

	OutputColorBuffer[launch_idx] 		= prd_texturing.Output.Color;
	OutputTexcoordBuffer[launch_idx] 	= prd_texturing.Output.Texcoord;
	OutputWeightBuffer[launch_idx] 		= prd_texturing.Output.Weight;
	OutputCamIdBuffer[launch_idx] 		= prd_texturing.Output.CamId;
	// uint2 lidx;
	// lidx.x = lidx.x / dim_mult;
	// lidx.y = lidx.y / dim_mult;
	// DebugBuffer[lidx]  = pix;
	
}