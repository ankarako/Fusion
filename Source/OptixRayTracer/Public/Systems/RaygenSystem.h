#ifndef	__OPTIXRAYTRACER_PUBLIC_SYSTEMS_RAYGENSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_RAYGENSYSTEM_H__

#include <Components/ContextComp.h>
#include <Components/RaygenProgComp.h>
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
	static void CreateRaygenComponent(
		RaygenProgComp raygenComp, 
		ContextComp ctxComp, 
		const std::string& ptxFilepath, 
		const std::string& progName)
	{
		raygenComp->RaygenProg = ctxComp->Context->createProgramFromPTXFile(ptxFilepath, progName);
	}
	///	\brief set the attributes of the raygen component
	///	\param	eye		the camera's eye
	///	\param	lookat	the camera's lookat
	///	\param	up		the camera's up
	static void SetRaygenAttributes(RaygenProgComp raygenComp, optix::float3 eye, optix::float3 lookat, optix::float3 up)
	{
		/// calculate camera plane
		raygenComp->RaygenProg["eye"]->setFloat(eye);
		raygenComp->RaygenProg["lookat"]->setFloat(lookat);
		raygenComp->RaygenProg["up"]->setFloat(up);
	}
};	///	!OptixPinholeRaygenSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_PINHOLERAYGENSYSTEM_H__