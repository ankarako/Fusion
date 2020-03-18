#ifndef	__OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__

#include <Components/ContextComp.h>
#include <Components/RaygenProgComp.h>
#include <Components/TriangleMeshComp.h>
#include <Components/MeshMaterialComp.h>
#include <Components/AccelerationComp.h>
#include <Components/PointCloudComp.h>
#include <thread>
#include <plog/Log.h>

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
	///	\brief attach point cloud to acceleration
	static void AttachPointCloudToAcceleration(PointCloudComp& pcComp, AccelerationComp& accelComp)
	{
		accelComp->GGroup->addChild(pcComp->GInstance);
	}
	/// \brief map acceleration comp dirty
	static void AccelerationCompMapDirty(AccelerationComp& accelComp)
	{
		accelComp->Acceleration->markDirty();
	}
	///	\brief map a mesh instance to a specified raygen program
	///	\param	mInstComp	the mesh instance component to set
	///	\param	raygenComp	the ray generation component to set to
	static void MapAccelerationToRaygen(AccelerationComp& mInstComp, RaygenProgComp& rayComp)
	{
		rayComp->RaygenProg["top_object"]->set(mInstComp->GGroup);
	}
	///	\brief Create a triagle mesh components's vertex buffer
	///	\param	meshComp	the mesh component to create its vertex buffer
	///	\param	ctxComp		the context component 
	///	\param	numVertices	the number of vertices
	static void CreateTriangleMeshVertexBuffer(TriangleMeshComp& meshComp, ContextComp& ctxComp, int numVertices)
	{
		meshComp->VertexBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, numVertices);
	}
	///	\brief Map vertices to a mesh's vertex buffer
	///	\paramn	meshComp	the mesh component
	///	\param	vertexData	the vertexData to copy
	///	\param	bytesize	the byte size of the vertex data
	static void MapTriangleMeshVertexBuffer(TriangleMeshComp& meshComp, void* vertexData, int byteSize)
	{
		std::memcpy(meshComp->VertexBuffer->map(), vertexData, byteSize);
		meshComp->VertexBuffer->unmap();
	}
	/// \brief Create a triangle mesh's normal buffer
	///	\param	meshComp	the mesh component
	///	\param	ctxComp		the context component
	///	\param	numVertices	the number of vertices of the mesh component
	static void CreateTriangleMeshNormalBuffer(TriangleMeshComp& meshComp, ContextComp& ctxComp, int numVertices)
	{
		meshComp->NormalBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, numVertices);
	}
	///	\brief map normals to a triangle mesh's normal buffer
	///	\param	meshComp	the mesh component
	///	\param	normalData	pointer to the normal data
	///	\param	byteSize	the byte sizE
	static void MapTriangleMeshNormalBuffer(TriangleMeshComp& meshComp, void* normalData, int byteSize)
	{
		std::memcpy(meshComp->NormalBuffer->map(), normalData, byteSize);
		meshComp->VertexBuffer->unmap();
	}
	///	\brief create a triangle mesh's face buffer
	///	\param	meshComp	the mesh component
	///	\param	ctxComp		the context component
	///	\param	numFaces	the number of faces
	static void CreateTriangleMeshFaceBuffer(TriangleMeshComp& meshComp, ContextComp& ctxComp, int numFaces)
	{
		meshComp->TIndexBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_INT3, numFaces);
	}
	///	\brief map face data to a triangle mesh
	///	\param	meshComp	the mesh component
	///	\param	faceData	pointer to the face data
	///	\param	byteSize	the byte size of the face data
	static void MapTriangleMeshFaceBuffer(TriangleMeshComp& meshComp, void* faceData, int byteSize)
	{
		std::memcpy(meshComp->TIndexBuffer->map(), faceData, byteSize);
		meshComp->TIndexBuffer->unmap();
	}
	///	\brief create a triangle mesh's texcoord buffer
	///	\param	meshComp	the mesh component
	///	\param	ctxComp		the context component
	///	\param	numCoords	the number of faces
	static void CreateTriangleMeshTexCoordBuffer(TriangleMeshComp& meshComp, ContextComp& ctxComp, int numCoords)
	{
		meshComp->TexCoordBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT2, numCoords);
	}
	///	\brief map texcoord data to a triangle mesh
	///	\param	meshComp	the mesh component
	///	\param	coordData	pointer to the face data
	///	\param	byteSize	the byte size of the face data
	static void MapTriangleMeshTexCoordBuffer(TriangleMeshComp& meshComp, void* coordData, int byteSize)
	{
		std::memcpy(meshComp->TexCoordBuffer->map(), coordData, byteSize);
		meshComp->TexCoordBuffer->unmap();
	}
	///	\brief create the vertex buffer of the specified pointcloud component
	///	\param	pcComp		the point cloud component
	///	\param	ctxComp		the context component
	///	\param	numVerts	the number of vertices in the buffer
	static void CreatePointCloudVertexBuffer(PointCloudComp& pcComp, ContextComp& ctxComp, int numVerts)
	{
		pcComp->VertexBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, numVerts);
	}
	///	\brief map vertices to a point cloud component's vertex buffer
	///	\param	pcComp		the point cloud component
	///	\param 	vertexData	pointer to the vertex data
	///	\param	bSize		the byte size of the vertex data
	static void MapPointCloudVertexBuffer(PointCloudComp& pcComp, void* data, int bSize)
	{
		std::memcpy(pcComp->VertexBuffer->map(), data, bSize);
		pcComp->VertexBuffer->unmap();
	}
	///	\brief create the normal buffer of the specified pointcloud component
	///	\param	pcComp		the point cloud component
	///	\param	ctxComp		the context component
	///	\param	numNorms	the number of vertices in the buffer
	static void CreatePointCloudNormalBuffer(PointCloudComp& pcComp, ContextComp& ctxComp, int numVerts)
	{
		pcComp->NormalBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, numVerts);
	}
	///	\brief map normals to a point cloud component's normal buffer
	///	\param	pcComp		the point cloud component
	///	\param 	normalData	pointer to the vertex data
	///	\param	bSize		the byte size of the vertex data
	static void MapPointCloudNormalBuffer(PointCloudComp& pcComp, void* data, int bSize)
	{
		std::memcpy(pcComp->NormalBuffer->map(), data, bSize);
		pcComp->NormalBuffer->unmap();
	}
	///	\brief create the color buffer of the specified pointcloud component
	///	\param	pcComp		the point cloud component
	///	\param	ctxComp		the context component
	///	\param	numColors	the number of vertices in the buffer
	static void CreatePointCloudColorBuffer(PointCloudComp& pcComp, ContextComp& ctxComp, int numColors)
	{
		pcComp->ColorBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE4, numColors);
	}
	///	\brief map colors to a point cloud component's color buffer
	///	\param	pcComp		the point cloud component
	///	\param 	data	pointer to the vertex data
	///	\param	bSize		the byte size of the vertex data
	static void MapPointCloudColorBuffer(PointCloudComp& pcComp, void* data, int bSize)
	{
		std::memcpy(pcComp->ColorBuffer->map(), data, bSize);
		pcComp->ColorBuffer->unmap();
	}
	///	\brief Map point cloud component
	static void MapPointCloudComponent(PointCloudComp& pcComp, ContextComp& ctxComp, int elementCount, bool hasnormals = false)
	{
		try {
			pcComp->Geometry = ctxComp->Context->createGeometry();
			pcComp->Material = ctxComp->Context->createMaterial();
			pcComp->GInstance = ctxComp->Context->createGeometryInstance();
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			pcComp->AnyHitProgram = ctxComp->Context->createProgramFromPTXFile(k_PointCloudPtxFilepath, k_PointCloudAnyHitProgName);
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			pcComp->ClosestHitProgram = ctxComp->Context->createProgramFromPTXFile(k_PointCloudPtxFilepath, k_PointCloudClosestHitProgName);
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			pcComp->BoundingBoxProgram = ctxComp->Context->createProgramFromPTXFile(k_PointCloudPtxFilepath, k_PointCloudBboxProgName);
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			pcComp->IntersectionProg = ctxComp->Context->createProgramFromPTXFile(k_PointCloudPtxFilepath, k_PointCloudIntersectionProgName);
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			
			///
			pcComp->Geometry->setBoundingBoxProgram(pcComp->BoundingBoxProgram);
			pcComp->Geometry->setIntersectionProgram(pcComp->IntersectionProg);
			pcComp->Material->setClosestHitProgram(0, pcComp->ClosestHitProgram);
			pcComp->Material->setAnyHitProgram(0, pcComp->AnyHitProgram);
			pcComp->GInstance->setGeometry(pcComp->Geometry);
			pcComp->GInstance->setMaterialCount(1);
			pcComp->GInstance->setMaterial(0, pcComp->Material);
			/// set the program's variables on ginstance 
			pcComp->GInstance["vertex_positions"]->setBuffer(pcComp->VertexBuffer);
			if (hasnormals)
				pcComp->GInstance["normal_buffer"]->setBuffer(pcComp->NormalBuffer);
			pcComp->GInstance["vertex_colors"]->setBuffer(pcComp->ColorBuffer);
			pcComp->GInstance["flat_shaded"]->setInt(1);
			pcComp->GInstance["radius"]->setFloat(0.01f);
			pcComp->Geometry->setPrimitiveCount(elementCount);
		}
		catch (optix::Exception& ex)
		{
			LOG_ERROR << "Failed to map PointCloud component. Error: " << ex.getErrorString();
		}
	}

	static void SetCullingPlanePos(PointCloudComp& pcComp, float cullPlanePos)
	{
		pcComp->GInstance["culling_plane_pos"]->setFloat(cullPlanePos);
	}
private:
	static constexpr const char* k_TriangleMeshPTxFilepath			= "FusionLib/Resources/Programs/TriangleMesh.ptx";
	static constexpr const char* k_TriangleMeshIntersectionProgName = "triangle_mesh_intersect";
	static constexpr const char* k_TriangleMeshBbboxProgName		= "triangle_mesh_bounds";
	static constexpr const char* k_PointCloudPtxFilepath 			= "FusionLib/Resources/Programs/PointCloud.ptx";
	static constexpr const char* k_PointCloudIntersectionProgName	= "pointcloud_intersect";
	static constexpr const char* k_PointCloudBboxProgName			= "pointcloud_bounds";
	static constexpr const char* k_PointCloudClosestHitProgName		= "pointcloud_closest_hit";
	static constexpr const char* k_PointCloudAnyHitProgName 		= "pointcloud_any_hit";
};	///	!class MeshMappingSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__