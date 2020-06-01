#include <optix_world.h>
#include <Helpers.cuh>
#include <RayPayload.cuh>

rtBuffer<optix::uchar4>					TextureBuffer;
rtTextureSampler<float4, 2>				TextureSampler;
rtBuffer<fu::rt::TexturingOutput, 2>		TexturingOutputBuffer;


rtDeclareVariable(optix::float3, eye, , );
rtDeclareVariable(optix::float3, up, , );
rtDeclareVariable(optix::float3, left, , );
rtDeclareVariable(optix::float3, lookat, , );
rtDeclareVariable(optix::float3, U, , );
rtDeclareVariable(optix::float3, V, , );
rtDeclareVariable(optix::float3, W, , );
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
	float u = (float)launch_idx.x / (float)launch_dims.x * 2.0f - 1.0f;
	float v = (float)launch_idx.y / (float)launch_dims.y * 2.0f - 1.0f;

	
	float4 color = tex2D(TextureSampler, v, u);
	uchar4 pix = fu::rt::make_color(optix::make_float3(color));
	
	fu::rt::PerRayData_Texturing prd_texturing;
	prd_texturing.PixelValue = pix;
	prd_texturing.CameraId = camId;
	prd_texturing.Output = { optix::make_uchar4(0, 0, 0, 0), optix::make_float2(0, 0), 0.0f, camId };

	optix::float3 ray_origin = eye;

	float fx = intrinsics.getRow(0).x;
	float fy = intrinsics.getRow(1).y;
	fx = fx / (float)launch_dims.x;
	fy = fy / (float)launch_dims.y;
	float4 u_dir = extrinsics * make_float4(u, 0.0f, 0.0f, 1.0f);
	float4 v_dir = extrinsics * make_float4(0.0f, v, 0.0f, 1.0f);
	float3 uu = make_float3(u_dir.x / u_dir.w, u_dir.y / u_dir.w, u_dir.z / u_dir.w);
	float3 vv = make_float3(v_dir.x / v_dir.w, v_dir.y / v_dir.w, v_dir.z / v_dir.w);
	
	optix::float3 ray_direction = optix::normalize(u * U + v * V + W);

	optix::Ray ray = optix::make_Ray(ray_origin, ray_direction, 0, scene_epsilon, RT_DEFAULT_MAX);
	rtTrace(top_object, ray, prd_texturing);

	TexturingOutputBuffer[launch_idx] = prd_texturing.Output;
}