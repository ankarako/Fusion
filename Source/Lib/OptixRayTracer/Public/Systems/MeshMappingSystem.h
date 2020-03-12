#ifndef	__OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__

#include <Components/ContextComp.h>
#include <Components/RaygenProgComp.h>
#include <Components/TriangleMeshComp.h>
#include <Components/MeshMaterialComp.h>
#include <Components/AccelerationComp.h>

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
	static void CreateMeshGeometry(AccelerationComp& geomComp, ContextComp& ctxComp)
	{
		
	}
	///	\brief initialize a null triangle mesh component
	///	\param	trComp	the triangle mesh component to initialize
	///	\param	ctxComp	the context component associated with the triangle mesh component
	static void NullInitializeTriangleMesh(TriangleMeshComp& trComp, ContextComp& ctxComp)
	{
		trComp->Geometry = ctxComp->Context->createGeometry();
		trComp->Material = ctxComp->Context->createMaterial();
		trComp->GInstance = ctxComp->Context->createGeometryInstance();
		trComp->VertexBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3);
		trComp->NormalBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3);
		trComp->TIndexBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_INT3);
		trComp->TexCoordBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_INT2);
		trComp->MaterialBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_INT);
		trComp->BoundingBoxProgram = ctxComp->Context->createProgramFromPTXFile(k_TriangleMeshPTxFilepath, k_TriangleMeshBbboxProgName);
		trComp->IntersectionProg = ctxComp->Context->createProgramFromPTXFile(k_TriangleMeshPTxFilepath, k_TriangleMeshIntersectionProgName);
		/// TODO: generate other programs as needed
		/// TODO: setup geometry completely
		trComp->Geometry->setBoundingBoxProgram(trComp->BoundingBoxProgram);
		trComp->Geometry->setIntersectionProgram(trComp->IntersectionProg);
		/// TODO: setup material
		/// attach geometry and material to geometry instance
		trComp->GInstance->setGeometry(trComp->Geometry);
		trComp->GInstance->setMaterialCount(1);
		trComp->GInstance->setMaterial(0, trComp->Material);
		/// set program variables (on Ginstance in order to not set them in each associated program - which is faster btw)
		trComp->GInstance["vertex_buffer"]->setBuffer(trComp->VertexBuffer);
		trComp->GInstance["normal_buffer"]->setBuffer(trComp->NormalBuffer);
		trComp->GInstance["tindex_buffer"]->setBuffer(trComp->TIndexBuffer);
		trComp->GInstance["texcoord_buffer"]->setBuffer(trComp->TexCoordBuffer);
		trComp->GInstance["material_buffer"]->setBuffer(trComp->MaterialBuffer);
		/// initialize geoemtry with zero primitive count
		trComp->Geometry->setPrimitiveCount(0);
	}
	///	\brief initialize a mesh instance component
	///	\param	mInstCom	the mesh instance component to initialize
	///	\param	ctxComp		the context component associated with the instance
	static void NullInitializeAcceleration(AccelerationComp& mInstComp, ContextComp& ctxComp)
	{
		mInstComp->GGroup = ctxComp->Context->createGeometryGroup();
		mInstComp->Acceleration = ctxComp->Context->createAcceleration("Trbvh");
		mInstComp->Transform = ctxComp->Context->createTransform();
		mInstComp->Transform->setChild(mInstComp->GGroup);
		mInstComp->GGroup->setAcceleration(mInstComp->Acceleration);
	}
	///	\brief attach a triangle mesh component to an acceleration component
	///	\param	trComp		the triangle mesh component to attach
	///	\param	accelComp	the acceleration component to attach the triangle mesh to
	static void AttachTriangleMeshToAcceleration(TriangleMeshComp& trComp, AccelerationComp& accelComp)
	{
		accelComp->GGroup->setChildCount(1);
		accelComp->GGroup->setChild(0, trComp->GInstance);
	}
	///	\brief map a mesh instance to a specified raygen program
	///	\param	mInstComp	the mesh instance component to set
	///	\param	raygenComp	the ray generation component to set to
	static void MapAccelerationToRaygen(AccelerationComp& mInstComp, RaygenProgComp& rayComp)
	{
		rayComp->RaygenProg["top_object"]->set(mInstComp->GGroup);
	}
private:
	static constexpr const char* k_TriangleMeshPTxFilepath			= "Resources/Programs/TriangleMesh.ptx";
	static constexpr const char* k_TriangleMeshIntersectionProgName = "triangle_mesh_intersect";
	static constexpr const char* k_TriangleMeshBbboxProgName		= "triangle_mesh_bounds";
};	///	!class MeshMappingSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__