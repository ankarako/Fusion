#ifndef	__OPTIXRAYTRACER_PUBLIC_SYSTEMS_RAYGENSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_RAYGENSYSTEM_H__

#include <Components/ContextComp.h>
#include <Components/RaygenProgComp.h>

#include <optix_world.h>
#include <string>

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
	static void CreateRaygenProg(
		RaygenProgComp raygenComp, 
		ContextComp ctxComp, 
		const std::string& ptxFilepath, 
		const std::string& progName,
		int width,
		int height)
	{
		raygenComp->RaygenProg = ctxComp->Context->createProgramFromPTXFile(ptxFilepath, progName);
		raygenComp->Eye		= optix::make_float3(0.0f, 0.0f, 0.0f);
		raygenComp->Lookat	= optix::make_float3(0.0f, 0.0f, 1.0f);
		raygenComp->Up		= optix::make_float3(0.0f, 1.0f, 0.0f);
		raygenComp->ViewWidth	= width;
		raygenComp->ViewHeight	= height;
		raygenComp->AspectRatio = static_cast<float>(width) / static_cast<float>(height);
	}
	/// \brief create a 360 ray generation component
	///	for convenience
	///	\param	rayComp	the raygen component
	///	\param	ctxComp	the context component
	static void Create360RaygenProg(RaygenProgComp& raygenComp, ContextComp& ctxComp, int width, int height)
	{
		raygenComp->RaygenProg = ctxComp->Context->createProgramFromPTXFile(k_360RaygenPtxFilepath, k_360RaygenProgName);
		raygenComp->Eye		= optix::make_float3(0.0f, 0.0f, 0.0f);
		raygenComp->Lookat	= optix::make_float3(0.0f, 0.0f, 1.0f);
		raygenComp->Up		= optix::make_float3(0.0f, 1.0f, 0.0f);
		raygenComp->ViewWidth	= width;
		raygenComp->ViewHeight	= height;
		raygenComp->AspectRatio = static_cast<float>(width) / static_cast<float>(height);
		ctxComp->Context->setRayTypeCount(1u);
		ctxComp->Context->setRayGenerationProgram(0u, raygenComp->RaygenProg);
		/// create output buffer
		raygenComp->OutputBuffer = ctxComp->Context->createBuffer(RT_BUFFER_OUTPUT, RT_FORMAT_UNSIGNED_BYTE4, width, height);
		raygenComp->RaygenProg["output_buffer"]->setBuffer(raygenComp->OutputBuffer);
	}
	///	\brief set the attributes of the raygen component
	///	\param	eye		the camera's eye
	///	\param	lookat	the camera's lookat
	///	\param	up		the camera's up
	static void SetRaygenAttributes(RaygenProgComp raygenComp)
	{
		/// calculate camera plane
		
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
	void CalcCameraPlaneBasis(optix::float3 eye, optix::float3 lookat, optix::float3 up, float aspectRatio, CameraPlaneBasis& camPlane)
	{
		float ulen;
		float vlen;
		float wlen;
		camPlane.W = lookat - eye;
		wlen = optix::length(camPlane.W);
		camPlane.U = optix::normalize(optix::cross(camPlane.W, camPlane.U));
		camPlane.V = optix::normalize(optix::cross(camPlane.U, camPlane.W));
		vlen = wlen * tanf(0.5f * m_Fov * M_PIf / 180.0f);
		camPlane.V *= vlen;
		ulen = vlen * aspectRatio;
		camPlane.U *= ulen;
	}
	///	\brief update camera parameters
	///	updates eye, lookat, up vectors
private:
	/// system state
	float m_Fov = 35.0f;
	static constexpr const char* k_360RaygenPtxFilepath = "Resources/Programs/EnvMapRaygen.ptx";
	static constexpr const char* k_360RaygenProgName = "EnvMapRaygen";
};	///	!OptixPinholeRaygenSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_PINHOLERAYGENSYSTEM_H__