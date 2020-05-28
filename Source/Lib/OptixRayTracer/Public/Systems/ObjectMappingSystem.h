#ifndef __OPTIXRAYTRACER_PUBLIC_SYSTEMS_OBJECTMAPPINGSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_OBJECTMAPPINGSYSTEM_H__

#include <RayTypes.h>
#include <ResourceTables.h>
#include <Components/TriangleMeshComp.h>
#include <Components/ContextComp.h>
#include <Components/AccelerationComp.h>
#include <optix_world.h>
#include <MeshData.h>

namespace fu {
namespace rt {
class ObjectMappingSystem
{
public:
	static void CreateTriangleMeshInstance(TriangleMeshComp trComp, ContextComp& ctxComp)
	{
		trComp->Geometry = ctxComp->Context->createGeometry();
		trComp->GInstance = ctxComp->Context->createGeometryInstance();
		trComp->GGroup = ctxComp->Context->createGeometryGroup();
		trComp->Acceleration = ctxComp->Context->createAcceleration("Trbvh");
		trComp->Transform = ctxComp->Context->createTransform();
		trComp->TransMat = optix::Matrix4x4::identity();
		trComp->Transform->setMatrix(false, trComp->TransMat.getData(), nullptr);
		trComp->GInstance->setGeometry(trComp->Geometry);
		trComp->GInstance->setMaterialCount(1);
		trComp->GInstance->setMaterial(0, trComp->Material);
		trComp->GGroup->setChildCount(1);
		trComp->GGroup->setChild(0, trComp->GInstance);
		trComp->GGroup->setAcceleration(trComp->Acceleration);
		trComp->Transform->setChild(trComp->GGroup);
	}

	static void MapGeometry(const io::MeshData& data, TriangleMeshComp& trComp, GeometryType hitType, VertexAttributes atts, ContextComp& ctxComp)
	{
		std::string ptxFilepath = GeometryProgramFilepaths.at(std::make_pair(hitType, atts));
		std::string boundsProgName = GeometryProgramNames.at(std::make_pair(hitType, GeometryProgType::BoundingBox));
		std::string intersectProgName = GeometryProgramNames.at(std::make_pair(hitType, GeometryProgType::Intersection));
		trComp->BoundingBoxProgram = ctxComp->Context->createProgramFromPTXFile(ptxFilepath, boundsProgName);
		trComp->IntersectionProg = ctxComp->Context->createProgramFromPTXFile(ptxFilepath, intersectProgName);
		trComp->Geometry->setBoundingBoxProgram(trComp->BoundingBoxProgram);
		trComp->Geometry->setIntersectionProgram(trComp->IntersectionProg);
	}

	static void MapMaterial(TriangleMeshComp& trComp, MaterialType matType, ContextComp& ctxComp)
	{
		std::string ptxFilepath = MaterialProgramFilepaths.at(matType);
		std::string closestHitName = MaterialProgramNames.at(std::make_pair(matType, MaterialProgType::ClosestHit));
		std::string anyhitName = MaterialProgramNames.at(std::make_pair(matType, MaterialProgType::AnyHit));
		trComp->ClosestHitProgram = ctxComp->Context->createProgramFromPTXFile(ptxFilepath, closestHitName);
		trComp->AnyHitProgram = ctxComp->Context->createProgramFromPTXFile(ptxFilepath, anyhitName);
		trComp->Material->setClosestHitProgram(0, trComp->ClosestHitProgram);
		trComp->Material->setAnyHitProgram(0, trComp->AnyHitProgram);
	}
};	///	!class ObjectMappingSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_OBJECTMAPPINGSYSTEM_H__