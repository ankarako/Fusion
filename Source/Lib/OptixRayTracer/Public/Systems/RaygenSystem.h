#ifndef	__OPTIXRAYTRACER_PUBLIC_SYSTEMS_RAYGENSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_RAYGENSYSTEM_H__

#include <Components/ContextComp.h>
#include <Components/RaygenProgComp.h>
#include <Buffer.h>
#include <Components/TexturingCameraComp.h>
#include <Components/AccelerationComp.h>
//#include <GL/gl3w.h>
#include <optix_world.h>
#include <optix_gl_interop.h>
#include <string>
#include <plog/Log.h>
#include <Programs/RayPayload.cuh>

namespace fu {
namespace rt {
///	\class OptixPinholeRaygenSystem
///	\brief pinhole camera creation and modification
class RaygenSystem
{
public:
	///	\brief create a raygeneration component
	///	\param	raygenComp	the raygen component to create
	///	\param	ctxComp		the context component associated with the raygen
	///	\param	ptxFilepath	the ptx source filepath
	///	\param	progName	the ray generation program name
	static void CreatePinholeRaygenProg(RaygenProgComp& raygenComp, ContextComp& ctxComp, int width, int height, float3 up)
	{
		raygenComp->RaygenProg = ctxComp->Context->createProgramFromPTXFile(k_PinholeRaygenPtxFilepath, k_PinholeRaygenProgName);
		raygenComp->Eye		= optix::make_float3(-6.0f, 1.0f, 1.0f);
		raygenComp->Lookat	= optix::make_float3(0.0f, 0.0f, 0.0f);
		raygenComp->Up		= optix::make_float3(up.x, up.y, up.z);
		raygenComp->ViewWidth	= width;
		raygenComp->ViewHeight	= height;
		raygenComp->AspectRatio = static_cast<float>(width) / static_cast<float>(height);
		raygenComp->Transform = optix::Matrix4x4::identity();
		/// TODO: maybe I ll have to change this
		ctxComp->Context->setRayTypeCount(1u);
		ctxComp->Context->setRayGenerationProgram(0u, raygenComp->RaygenProg);
		/// create output buffer
		raygenComp->OutputBuffer = ctxComp->Context->createBuffer(RT_BUFFER_OUTPUT, RT_FORMAT_UNSIGNED_BYTE4, width, height);
		raygenComp->RaygenProg["output_buffer"]->setBuffer(raygenComp->OutputBuffer);
		SetRaygenAttributes(raygenComp);
	}
	/// \brief create a 360 ray generation component
	///	for convenience
	///	\param	rayComp	the raygen component
	///	\param	ctxComp	the context component
	static void Create360RaygenProg(RaygenProgComp& raygenComp, ContextComp& ctxComp, int width, int height)
	{
		raygenComp->RaygenProg = ctxComp->Context->createProgramFromPTXFile(k_360RaygenPtxFilepath, k_360RaygenProgName);
		raygenComp->Eye		= optix::make_float3(0.0f, 0.0f, 0.0f);
		raygenComp->Lookat	= optix::make_float3(1.0f, 0.0f, 0.0f);
		raygenComp->Up		= optix::make_float3(0.0f, -1.0f, 0.0f);
		raygenComp->ViewWidth	= width;
		raygenComp->ViewHeight	= height;
		raygenComp->AspectRatio = static_cast<float>(width) / static_cast<float>(height);
		raygenComp->Transform = optix::Matrix4x4::identity();
		ctxComp->Context->setRayTypeCount(1u);
		ctxComp->Context->setRayGenerationProgram(0u, raygenComp->RaygenProg);
		/// create output buffer
		raygenComp->OutputBuffer = ctxComp->Context->createBuffer(RT_BUFFER_OUTPUT, RT_FORMAT_UNSIGNED_BYTE4, width, height);
		raygenComp->RaygenProg["output_buffer"]->setBuffer(raygenComp->OutputBuffer);
	}
	///	\brief create a 360 ray generation program with a specified output buffer
	///	\param	rayComp		the raygen component
	///	\param	ctxComp		the context component
	///	\param	size		the launchSize
	///	\param	pboHandle	the pixel buffer handle
	//static void Create360RaygenProgWithPBO(RaygenProgComp& raygenComp, ContextComp& ctxComp, uint2 size, GLuint pboHandle)
	//{
	//	raygenComp->RaygenProg = ctxComp->Context->createProgramFromPTXFile(k_360RaygenPtxFilepath, k_360RaygenProgName);
	//	raygenComp->Eye		= optix::make_float3(0.0f, 0.0f, 0.0f);
	//	raygenComp->Lookat	= optix::make_float3(1.0f, 0.0f, 0.0f);
	//	raygenComp->Up		= optix::make_float3(0.0f, 1.0f, 0.0f);
	//	raygenComp->ViewWidth	= size.x;
	//	raygenComp->ViewHeight	= size.y;
	//	raygenComp->AspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
	//	raygenComp->Transform = optix::Matrix4x4::identity();
	//	ctxComp->Context->setRayTypeCount(1u);
	//	ctxComp->Context->setRayGenerationProgram(0u, raygenComp->RaygenProg);
	//	/// output from pbo
	//	try {
	//		raygenComp->OutputBuffer = ctxComp->Context->createBufferFromGLBO(RT_BUFFER_OUTPUT, pboHandle);
	//	}
	//	catch (optix::Exception& ex)
	//	{
	//		LOG_ERROR << ex.getErrorString();
	//	}
	//	raygenComp->OutputBuffer->setFormat(RT_FORMAT_UNSIGNED_BYTE4);
	//	raygenComp->OutputBuffer->setSize(size.x, size.y);
	//	raygenComp->RaygenProg["output_buffer"]->setBuffer(raygenComp->OutputBuffer);
	//}
	///	\brief set the attributes of the raygen component
	///	\param	eye		the camera's eye
	///	\param	lookat	the camera's lookat
	///	\param	up		the camera's up
	static void SetRaygenAttributes(RaygenProgComp& raygenComp)
	{
		CameraPlaneBasis basis;
		UpdateCameraParams(raygenComp, basis);
		raygenComp->RaygenProg["U"]->setFloat(basis.U);
		raygenComp->RaygenProg["V"]->setFloat(basis.V);
		raygenComp->RaygenProg["W"]->setFloat(basis.W);
		raygenComp->RaygenProg["eye"]->setFloat(raygenComp->Eye);
	}
	///	\brief set the attributes of the raygen component
	///	\param	eye		the camera's eye
	///	\param	lookat	the camera's lookat
	///	\param	up		the camera's up
	static void SetRaygenAttributes(TexturingCameraComp& raygenComp)
	{
		CameraPlaneBasis basis;
		UpdateCameraParams(raygenComp, basis);
		raygenComp->RaygenProg["U"]->setFloat(basis.U);
		raygenComp->RaygenProg["V"]->setFloat(basis.V);
		raygenComp->RaygenProg["W"]->setFloat(basis.W);
		raygenComp->RaygenProg["eye"]->setFloat(raygenComp->Eye);
	}

	static void SetPinholeRaygenTransMat(RaygenProgComp& raygenComp, const optix::Matrix4x4& mat)
	{
		raygenComp->Transform = mat;
		SetRaygenAttributes(raygenComp);
	}

	static void SetPinholeRaygenEyeTranslation(RaygenProgComp& raygenComp, const optix::float3& trans)
	{
		raygenComp->Eye += trans;
		SetRaygenAttributes(raygenComp);
	}

	static void ChangePinholeRaygenDimensions(RaygenProgComp& raygenComp, const int width, const int height)
	{
		raygenComp->ViewWidth = width;
		raygenComp->ViewHeight = height;
		raygenComp->AspectRatio = width / height;
		raygenComp->OutputBuffer->setSize(width, height);
	}

	static void MapTexturingCamera(TexturingCameraComp& camComp, ContextComp& ctxComp, const optix::uint2& camPlaneSize, int camId, int launchDimsMult)
	{
		camComp->Id = camId;
		camComp->CamPlaneWidth = camPlaneSize.x;
		camComp->CamPlaneHeight = camPlaneSize.y;
		camComp->AspectRatio = (float)camComp->CamPlaneWidth / (float)camComp->CamPlaneHeight;
		/// create Raygen Program
		camComp->RaygenProg = ctxComp->Context->createProgramFromPTXFile(k_TexturingRaygenPtxFilepath, k_TexturingRaygenProgName);
		/// create Camera Texture Buffer and Texture Sampler
		camComp->TextureBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE4, camPlaneSize.x, camPlaneSize.y);
		camComp->TextureSampler = ctxComp->Context->createTextureSampler();
		camComp->TextureSampler->setBuffer(camComp->TextureBuffer);
		camComp->TextureSampler->setFilteringModes(RT_FILTER_NEAREST, RT_FILTER_NEAREST, RT_FILTER_NEAREST);
		camComp->TextureSampler->setMaxAnisotropy(1.0f);
		camComp->TextureSampler->setIndexingMode(RT_TEXTURE_INDEX_ARRAY_INDEX);
		/// map program variables
		camComp->RaygenProg["scene_epsilon"]->setFloat(1e-3f);
		camComp->RaygenProg["TextureBuffer"]->setBuffer(camComp->TextureBuffer);
		camComp->RaygenProg["TextureSampler"]->setTextureSampler(camComp->TextureSampler);
		camComp->RaygenProg["camId"]->setInt(camComp->Id);
		camComp->RaygenProg["extrinsics"]->setMatrix4x4fv(false, camComp->Extrinsics.getData());
		camComp->RaygenProg["intrinsics"]->setMatrix4x4fv(false, camComp->Intrinsics.getData());
		camComp->RaygenProg["dim_mult"]->setUint(launchDimsMult);
		/// output buffers
		camComp->OutputColorBuffer = 
			ctxComp->Context->createBuffer(
				RT_BUFFER_OUTPUT, 
				RT_FORMAT_UNSIGNED_BYTE4, 
				camPlaneSize.x * launchDimsMult, 
				camPlaneSize.y * launchDimsMult);
		camComp->OutputTexcoordBuffer = 
			ctxComp->Context->createBuffer(
				RT_BUFFER_OUTPUT, 
				RT_FORMAT_FLOAT2, 
				camPlaneSize.x * launchDimsMult, 
				camPlaneSize.y * launchDimsMult);
		camComp->OutputWeightBuffer = 
			ctxComp->Context->createBuffer(
				RT_BUFFER_OUTPUT, 
				RT_FORMAT_FLOAT, 
				camPlaneSize.x * launchDimsMult, 
				camPlaneSize.y * launchDimsMult);
		camComp->OutputCamIdBuffer = 
			ctxComp->Context->createBuffer(
				RT_BUFFER_OUTPUT, 
				RT_FORMAT_INT, 
				camPlaneSize.x * launchDimsMult, 
				camPlaneSize.y * launchDimsMult);
		/// map output buffers to program
		camComp->RaygenProg["OutputColorBuffer"]->setBuffer(camComp->OutputColorBuffer);
		camComp->RaygenProg["OutputTexcoordBuffer"]->setBuffer(camComp->OutputTexcoordBuffer);
		camComp->RaygenProg["OutputWeightBuffer"]->setBuffer(camComp->OutputWeightBuffer);
		camComp->RaygenProg["OutputCamIdBuffer"]->setBuffer(camComp->OutputCamIdBuffer);
		/// set ray generation program to context
		ctxComp->Context->setRayGenerationProgram(camId, camComp->RaygenProg);
		// TODO: delete the debug buffer
		//camComp->DebugBuffer = 
		//	ctxComp->Context->createBuffer(
		//		RT_BUFFER_OUTPUT, 
		//		RT_FORMAT_UNSIGNED_BYTE4, 
		//		camPlaneSize.x, 
		//		camPlaneSize.y);
		//camComp->RaygenProg["DebugBuffer"]->setBuffer(camComp->DebugBuffer);
	}

	static void TexturingCameraUpdateTexture(TexturingCameraComp& camComp, const BufferCPU<optix::uchar4>& text)
	{
		int bsize = text->ByteSize();
		std::memcpy(camComp->TextureBuffer->map(), text->Data(), bsize);
		camComp->TextureBuffer->unmap();
	}

	static void AttachTopLevelAccelerationToTexturingRaygen(TexturingCameraComp& camComp, AccelerationComp& acc)
	{
		camComp->RaygenProg["top_object"]->set(acc->Group);
	}
private:
	///	\struct CameraPlaneBasis
	///	\brief helper struct with the vectors that define a camera plane
	struct CameraPlaneBasis
	{
		optix::float3 U;
		optix::float3 V;
		optix::float3 W;
	};
	///	\brief calculate camera parameters from given eye, llokat and up vectors
	///	\param	eye			the eye of the camera
	///	\param	lookat		the lookat vector of the camera
	///	\param	up			the up vector of the camera
	///	\param	aspectRatio	the aspect ration of the camera
	///	\param[out] the CameraPlaneBasis struct
	static void CalcCameraPlaneBasis(optix::float3& eye, optix::float3& lookat, optix::float3& up, float aspectRatio, CameraPlaneBasis& camPlane)
	{
		float ulen;
		float vlen;
		float wlen;
		camPlane.W = lookat - eye;
		wlen = optix::length(camPlane.W);
		camPlane.U = optix::normalize(optix::cross(camPlane.W, up));
		camPlane.V = optix::normalize(optix::cross(camPlane.U, camPlane.W));
		vlen = wlen * tanf(0.5f * m_Fov * M_PIf / 180.0f);
		camPlane.V *= vlen;
		ulen = vlen * aspectRatio;
		camPlane.U *= ulen;
	}
	///	\brief update camera parameters
	///	updates eye, lookat, up vectors
	///	\param	raygenComp	the raygen component to update
	///	\param	camPlane	the image plane basis vectors
	void static UpdateCameraParams(RaygenProgComp& raygenComp, CameraPlaneBasis& camPlane)
	{
		CalcCameraPlaneBasis(raygenComp->Eye, raygenComp->Lookat, raygenComp->Up, raygenComp->AspectRatio, camPlane);
		const optix::Matrix4x4 frame	= optix::Matrix4x4::fromBasis(optix::normalize(camPlane.U), optix::normalize(camPlane.V), optix::normalize(-camPlane.W), raygenComp->Lookat);
		const optix::Matrix4x4 frameInv = frame.inverse();
		const optix::Matrix4x4 trans	= frame * raygenComp->Transform * frameInv;
		const optix::float3 eye			= raygenComp->Eye;
		const optix::float3 lookat		= raygenComp->Lookat;
		const optix::float3 up			= raygenComp->Up;
		raygenComp->Eye		= optix::make_float3(trans * optix::make_float4(eye, 1.0f));
		raygenComp->Lookat	= optix::make_float3(trans * optix::make_float4(lookat, 1.0f));
		raygenComp->Up		= optix::make_float3(trans * optix::make_float4(up, 1.0f));
		//CalcCameraPlaneBasis(raygenComp->Eye, raygenComp->Lookat, raygenComp->Up, raygenComp->AspectRatio, camPlane);
	}
	///	\brief update camera parameters
	///	updates eye, lookat, up vectors
	///	\param	raygenComp	the raygen component to update
	///	\param	camPlane	the image plane basis vectors
	void static UpdateCameraParams(TexturingCameraComp& raygenComp, CameraPlaneBasis& camPlane)
	{
		float ulen;
		float vlen;
		float wlen;
		camPlane.W = -raygenComp->LookAt - raygenComp->Eye;
		wlen = optix::length(camPlane.W);
		camPlane.U = optix::normalize(optix::cross(camPlane.W, raygenComp->Up));
		camPlane.V = optix::normalize(optix::cross(camPlane.U, camPlane.W));
		float f = raygenComp->Intrinsics.getRow(0).x / raygenComp->CamPlaneWidth;
		vlen = wlen * tanf(0.5f * 90.0f * M_PIf / 180.0f);
		camPlane.V *= vlen;
		ulen = vlen * raygenComp->AspectRatio;
		camPlane.U *= ulen;
		const optix::Matrix4x4 frame = optix::Matrix4x4::fromBasis(optix::normalize(camPlane.U), optix::normalize(camPlane.V), optix::normalize(-camPlane.W), raygenComp->LookAt);
		const optix::Matrix4x4 frameInv = frame.inverse();
		const optix::Matrix4x4 trans = frame * raygenComp->Extrinsics * frameInv;
		const optix::float3 eye = raygenComp->Eye;
		const optix::float3 lookat = raygenComp->LookAt;
		const optix::float3 up = raygenComp->Up;
		raygenComp->Eye = optix::make_float3(trans * optix::make_float4(eye, 1.0f));
		raygenComp->LookAt = optix::make_float3(trans * optix::make_float4(lookat, 1.0f));
		raygenComp->Up = optix::make_float3(trans * optix::make_float4(up, 1.0f));
		//CalcCameraPlaneBasis(raygenComp->Eye, raygenComp->LookAt, raygenComp->Up, raygenComp->AspectRatio, camPlane);
	}
private:
	/// system state
	static constexpr float m_Fov = 35.0f;
	static constexpr const char* k_360RaygenPtxFilepath			= "FusionLib/Resources/Programs/EnvMapRaygen.ptx";
	static constexpr const char* k_360RaygenProgName			= "EnvMapRaygen";
	static constexpr const char* k_PinholeRaygenPtxFilepath		= "FusionLib/Resources/Programs/PinholeRaygen.ptx";
	static constexpr const char* k_PinholeRaygenProgName		= "PinholeRaygen";
	static constexpr const char* k_TexturingRaygenPtxFilepath	= "FusionLib/Resources/Programs/TexturingRaygen.ptx";
	static constexpr const char* k_TexturingRaygenProgName		= "TexturingRaygen";
};	///	!OptixPinholeRaygenSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_PINHOLERAYGENSYSTEM_H__