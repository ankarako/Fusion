#ifndef	__OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__

#include <Components/ContextComp.h>
#include <Components/RaygenProgComp.h>
#include <Components/MeshGeometryComp.h>
#include <Components/MeshMaterialComp.h>
#include <Components/MeshInstanceComp.h>

namespace fu {
namespace rt {
///	\class MeshMappingSystem
///	\brief	provides mesh input functionalities to optix
class MeshMappingSystem
{
public:
	///	\brief create a geometry component
	///	\param	geomComp	the geomtry component to create
	///	\param	ctxComp		the optix context to associate with the geoemtry
	static void CreateMeshGeometry(MeshInstanceComp geomComp, ContextComp ctxComp)
	{
		
	}
	///	\brief initialize a mesh instance component
	///	\param	mInstCom	the mesh instance component to initialize
	///	\param	ctxComp		the context component associated with the instance
	static void NullInitializeMeshInstance(MeshInstanceComp& mInstComp, ContextComp& ctxComp)
	{
		mInstComp->GGroup = ctxComp->Context->createGeometryGroup();
		mInstComp->Acceleration = ctxComp->Context->createAcceleration("Trbvh");
		mInstComp->GInstance = ctxComp->Context->createGeometryInstance();
		mInstComp->Transform = ctxComp->Context->createTransform();
	}
	///	\brief map a mesh instance to a specified raygen program
	///	\param	mInstComp	the mesh instance component to set
	///	\param	raygenComp	the ray generation component to set to
	static void MapMeshInstanceToRaygen(MeshInstanceComp& mInstComp, RaygenProgComp& rayComp)
	{
		rayComp->RaygenProg["top_object"]->set(mInstComp->GGroup);
	}
};	///	!class MeshMappingSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__