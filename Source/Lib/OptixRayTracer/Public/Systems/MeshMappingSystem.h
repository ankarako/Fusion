#ifndef	__OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__

#include <Components/ContextComp.h>
#include <Components/RaygenProgComp.h>
#include <Components/TriangleMeshComp.h>
#include <Components/MeshMaterialComp.h>
#include <Components/AccelerationComp.h>
#include <Components/PointCloudComp.h>
#include <Systems/ResourseDesc.h>
#include <MeshData.h>
#include <thread>
#include <plog/Log.h>

namespace fu {
namespace rt {
enum class MaterialType : unsigned short
{
	Unknown = 0,
	Phong,
	SolidColor,
	VertexColor
};
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
		trComp->GGroup = ctxComp->Context->createGeometryGroup();
		trComp->Transform = ctxComp->Context->createTransform();
		trComp->VertexBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3);
		trComp->NormalBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3);
		trComp->ColorsBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE4);
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
		trComp->GGroup->addChild(trComp->GInstance);
		trComp->Transform->setChild(trComp->GGroup);
		trComp->Acceleration = ctxComp->Context->createAcceleration("Trbvh");
		trComp->GGroup->setAcceleration(trComp->Acceleration);
		/// set program variables (on Ginstance in order to not set them in each associated program - which is faster btw)
		trComp->GInstance["vertex_buffer"]->setBuffer(trComp->VertexBuffer);
		trComp->GInstance["normal_buffer"]->setBuffer(trComp->NormalBuffer);
		trComp->GInstance["tindex_buffer"]->setBuffer(trComp->TIndexBuffer);
		trComp->GInstance["color_buffer"]->setBuffer(trComp->ColorsBuffer);
		trComp->GInstance["texcoord_buffer"]->setBuffer(trComp->TexCoordBuffer);
		trComp->GInstance["material_buffer"]->setBuffer(trComp->MaterialBuffer);
		trComp->BBox = optix::Aabb(optix::make_float3(1.e16f, 1.e16f, 1.e16f), optix::make_float3(-1.e16f, -1.e16f, -1.e16f));
		/// initialize geoemtry with zero primitive count
		trComp->Geometry->setPrimitiveCount(0);
	}

	static void MapTriangleMeshComponent(TriangleMeshComp& trComp, ContextComp& ctxComp, int elmntCount, bool hasnormals = false)
	{
		try
		{
			trComp->Acceleration = ctxComp->Context->createAcceleration("Trbvh");
			trComp->Transform = ctxComp->Context->createTransform();
			trComp->Geometry = ctxComp->Context->createGeometry();
			trComp->Material = ctxComp->Context->createMaterial();
			trComp->GInstance = ctxComp->Context->createGeometryInstance();
			trComp->AnyHitProgram = ctxComp->Context->createProgramFromPTXFile(k_SolidColorPtxFilepath, k_TriangleMeshSolidColorAnyHitProgName);
			trComp->ClosestHitProgram = ctxComp->Context->createProgramFromPTXFile(k_SolidColorPtxFilepath, k_TriangleMeshSolidColorClosestHitProgName);
			trComp->BoundingBoxProgram = ctxComp->Context->createProgramFromPTXFile(k_TriangleMeshPTxFilepath, k_TriangleMeshBbboxProgName);
			trComp->IntersectionProg = ctxComp->Context->createProgramFromPTXFile(k_TriangleMeshPTxFilepath, k_TriangleMeshIntersectionProgName);

			trComp->Geometry->setBoundingBoxProgram(trComp->BoundingBoxProgram);
			trComp->Geometry->setIntersectionProgram(trComp->IntersectionProg);
			trComp->Material->setClosestHitProgram(0, trComp->ClosestHitProgram);
			trComp->Material->setAnyHitProgram(0, trComp->AnyHitProgram);
			trComp->GInstance->setGeometry(trComp->Geometry);
			trComp->GInstance->setMaterialCount(1);
			trComp->GInstance->setMaterial(0, trComp->Material);
			///
			trComp->GInstance["vertex_buffer"]->setBuffer(trComp->VertexBuffer);
			if (hasnormals)
				trComp->GInstance["normal_buffer"]->setBuffer(trComp->NormalBuffer);
			trComp->GInstance["tindex_buffer"]->setBuffer(trComp->TIndexBuffer);
			trComp->GInstance["solid_color"]->setFloat(optix::make_float3(0.5f, 0.5f, 0.5f));
			trComp->Geometry->setPrimitiveCount(elmntCount);

			
			optix::Matrix4x4 trans = optix::Matrix4x4::identity();
			trComp->Transform->setMatrix(false, trans.getData(), nullptr);
			trComp->GGroup = ctxComp->Context->createGeometryGroup();
			trComp->GGroup->addChild(trComp->GInstance);
			trComp->GGroup->setAcceleration(trComp->Acceleration);
			trComp->Transform->setChild(trComp->GGroup);
		}
		catch (optix::Exception & ex)
		{
			LOG_ERROR << "Failed to map triangle mesh component: " << ex.getErrorString();
			return;
		}
	}
	///	\brief initialize a mesh instance component
	///	\param	mInstCom	the mesh instance component to initialize
	///	\param	ctxComp		the context component associated with the instance
	static void NullInitializeAcceleration(AccelerationComp& mInstComp, ContextComp& ctxComp)
	{
		mInstComp->Group = ctxComp->Context->createGroup();
		mInstComp->Acceleration = ctxComp->Context->createAcceleration("Trbvh");
		mInstComp->Group->setAcceleration(mInstComp->Acceleration);
	}
	///	\brief attach a triangle mesh component to an acceleration component
	///	\param	trComp		the triangle mesh component to attach
	///	\param	accelComp	the acceleration component to attach the triangle mesh to
	static void AttachTriangleMeshToAcceleration(TriangleMeshComp& trComp, AccelerationComp& accelComp)
	{
		accelComp->Group->addChild(trComp->GGroup);
		//accelComp->GGroup->setChild(0, trComp->GInstance);
	}
	///	\brief attach point cloud to acceleration
	//static void AttachPointCloudToAcceleration(PointCloudComp& pcComp, AccelerationComp& accelComp)
	//{
	//	accelComp->Group->addChild(pcComp->GGroup);
	//}
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
		rayComp->RaygenProg["top_object"]->set(mInstComp->Group);
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
			pcComp->AnyHitProgram = ctxComp->Context->createProgramFromPTXFile(k_PointCloudPtxFilepath, k_PointCloudAnyHitProgName);
			pcComp->ClosestHitProgram = ctxComp->Context->createProgramFromPTXFile(k_PointCloudPtxFilepath, k_PointCloudClosestHitProgName);
			pcComp->BoundingBoxProgram = ctxComp->Context->createProgramFromPTXFile(k_PointCloudPtxFilepath, k_PointCloudBboxProgName);
			pcComp->IntersectionProg = ctxComp->Context->createProgramFromPTXFile(k_PointCloudPtxFilepath, k_PointCloudIntersectionProgName);
			
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
	///	\brief map mesh data to point cloud
	/// TODO: Actually there should be only a MapMeshData and resolving to point cloud should be done by the mesh system
	///	\param	data	the mesh data to map
	///	\param	pcComp	the PointcloudComponent to map the mesh data to
	///	\param	ctxComp	the context component that will map the data
	static void MapMeshDataToPointCloud(const io::MeshData& data, PointCloudComp& pcComp, ContextComp& ctxComp)
	{
		pcComp->Geometry		= ctxComp->Context->createGeometry();
		pcComp->GInstance		= ctxComp->Context->createGeometryInstance();
		pcComp->Material		= ctxComp->Context->createMaterial();
		pcComp->GGroup			= ctxComp->Context->createGeometryGroup();
		pcComp->Acceleration	= ctxComp->Context->createAcceleration("Trbvh");
		pcComp->Transform		= ctxComp->Context->createTransform();
		std::string inter_ptx_path;
		std::string mat_ptx_path;
		///=================
		/// Copy Data
		///=================
		/// we expect that every piece of mesh data has vertices
		{
			int count = data->VertexBuffer->Count();
			int bsize = data->VertexBuffer->ByteSize();
			pcComp->VertexBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, count);
			std::memcpy(pcComp->VertexBuffer->map(), data->VertexBuffer->Data(), bsize);
			pcComp->VertexBuffer->unmap();
			pcComp->GInstance["vertex_positions"]->setBuffer(pcComp->VertexBuffer);
		}
		std::string intersect = "Pointcloud";
		std::string attributes = "NoAttrib";
		/// let's see about normals
		if (data->HasNormals)
		{
			int count = data->NormalBuffer->Count();
			int bsize = data->NormalBuffer->ByteSize();
			pcComp->NormalBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, count);
			std::memcpy(pcComp->NormalBuffer->map(), data->NormalBuffer->Data(), bsize);
			pcComp->NormalBuffer->unmap();
			pcComp->GInstance["normal_buffer"]->setBuffer(pcComp->NormalBuffer);
			if (attributes == "NoAttrib")
			{
				attributes = "Normal";
			}
			else
			{
				attributes += "Normal";
			}
			
		}
		if (data->HasColors)
		{
			int count = data->ColorBuffer->Count();
			int bsize = data->ColorBuffer->ByteSize();
			pcComp->ColorBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE4, count);
			std::memcpy(pcComp->ColorBuffer->map(), data->ColorBuffer->Data(), bsize);
			pcComp->ColorBuffer->unmap();
			pcComp->GInstance["vertex_colors"]->setBuffer(pcComp->ColorBuffer);
			if (attributes == "NoAttrib")
			{
				attributes = "Color";
			}
			else
			{
				attributes += "Color";
			}
		}
		pcComp->GInstance["flat_shaded"]->setInt(1);
		pcComp->GInstance["radius"]->setFloat(0.01f);
		pcComp->GInstance["culling_plane_pos"]->setFloat(10.0f);
		///=========================
		/// Create & Attach Programs
		///=========================
		std::string ptxFilepath		= GetResourceFilepath(intersect, attributes);
		std::string intersection	= GetResourceProgName(intersect + "Intersection", attributes);
		std::string bounding		= GetResourceProgName(intersect + "BoundingBox", attributes);
		std::string closest			= GetResourceProgName(intersect + "ClosestHit", attributes);
		std::string any				= GetResourceProgName(intersect + "AnyHit", attributes);
		pcComp->IntersectionProg	= ctxComp->Context->createProgramFromPTXFile(ptxFilepath, intersection);
		pcComp->BoundingBoxProgram	= ctxComp->Context->createProgramFromPTXFile(ptxFilepath, bounding);
		pcComp->ClosestHitProgram	= ctxComp->Context->createProgramFromPTXFile(ptxFilepath, closest);
		pcComp->AnyHitProgram		= ctxComp->Context->createProgramFromPTXFile(ptxFilepath, any);
		pcComp->Geometry->setIntersectionProgram(pcComp->IntersectionProg);
		pcComp->Geometry->setBoundingBoxProgram(pcComp->BoundingBoxProgram);
		pcComp->Geometry->setPrimitiveCount(data->VertexBuffer->Count());
		pcComp->Material->setClosestHitProgram(0, pcComp->ClosestHitProgram);
		pcComp->Material->setAnyHitProgram(0, pcComp->AnyHitProgram);
		pcComp->GInstance->setGeometry(pcComp->Geometry);
		pcComp->GInstance->setMaterialCount(1);
		pcComp->GInstance->setMaterial(0, pcComp->Material);
		pcComp->GGroup->addChild(pcComp->GInstance);
		pcComp->Transform->setChild(pcComp->GGroup);
		pcComp->GGroup->setAcceleration(pcComp->Acceleration);
	}
	///	\brief attach a point cloud component to a top level accelration component
	///	\param	pccomp		the point cloud component to attach
	///	\param	acclComp	the top level acceleration component to attach the point cloud
	void static AttachPointCloudCompToTopLevelAcceleration(PointCloudComp& pcComp, AccelerationComp& accelComp)
	{
		accelComp->Group->addChild(pcComp->GGroup);
		accelComp->Acceleration->markDirty();
	}
	///	\brief map mesh data to a triangle mesh component
	///	\param	data	the mesh data to map
	///	\param	trComp	the triangle mesh component to map the data to
	///	\param	ctxComp	the context component that will map the data
	///	\param	matType	the material type of the triangle mesh component
	static void MapMeshDataToTriangleMesh(const io::MeshData& data, TriangleMeshComp& trComp, ContextComp& ctxComp)
	{
		trComp->Geometry		= ctxComp->Context->createGeometry();
		trComp->GInstance		= ctxComp->Context->createGeometryInstance();
		trComp->Material		= ctxComp->Context->createMaterial();
		trComp->GGroup			= ctxComp->Context->createGeometryGroup();
		trComp->Acceleration	= ctxComp->Context->createAcceleration("Trbvh");
		trComp->Transform	= ctxComp->Context->createTransform();
		///==========
		/// Copy data
		///==========
		std::string intersect = "TriangleMesh";
		std::string attributes = "NoAttrib";
		/// we expect that every piece of mesh data has vertices
		{
			int count = data->VertexBuffer->Count();
			int bsize = data->VertexBuffer->ByteSize();
			trComp->VertexBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, count);
			std::memcpy(trComp->VertexBuffer->map(), data->VertexBuffer->Data(), bsize);
			trComp->VertexBuffer->unmap();
			trComp->GInstance["vertex_buffer"]->setBuffer(trComp->VertexBuffer);
		}
		/// here we expect that mesh data always carry faces
		{
			int count = data->TIndexBuffer->Count();
			int bsize = data->TIndexBuffer->ByteSize();
			trComp->TIndexBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_INT3, count);
			std::memcpy(trComp->TIndexBuffer->map(), data->TIndexBuffer->Data(), bsize);
			trComp->TIndexBuffer->unmap();
			trComp->GInstance["tindex_buffer"]->setBuffer(trComp->TIndexBuffer);
		}
		if (data->HasNormals)
		{
			int count = data->NormalBuffer->Count();
			int bsize = data->NormalBuffer->ByteSize();
			trComp->NormalBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, count);
			std::memcpy(trComp->NormalBuffer->map(), data->VertexBuffer->Data(), bsize);
			trComp->NormalBuffer->unmap();
			trComp->GInstance["normal_buffer"]->setBuffer(trComp->NormalBuffer);
			if (attributes == "NoAttrib")
			{
				attributes = "Normals";
			}
			else
			{
				attributes += "Normals";
			}
		}
		else
		{
			trComp->NormalBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, 0);
			trComp->GInstance["normal_buffer"]->setBuffer(trComp->NormalBuffer);
		}
		if (data->HasColors)
		{
			int count = data->ColorBuffer->Count();
			int bsize = data->ColorBuffer->ByteSize();
			trComp->ColorsBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE4, count);
			std::memcpy(trComp->ColorsBuffer->map(), data->ColorBuffer->Data(), bsize);
			trComp->ColorsBuffer->unmap();
			trComp->GInstance["color_buffer"]->setBuffer(trComp->ColorsBuffer);
			if (attributes == "NoAttrib")
			{
				attributes = "Color";
			}
			else
			{
				attributes += "Color";
			}
		}
		else
		{
			trComp->ColorsBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE4, 0);
			trComp->GInstance["color_buffer"]->setBuffer(trComp->ColorsBuffer);
		}
		if (data->HasTexcoords)
		{
			int count = data->TexcoordBuffer->Count();
			int bsize = data->TexcoordBuffer->ByteSize();
			trComp->TexCoordBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT2, count);
			std::memcpy(trComp->TexCoordBuffer->map(), data->TexcoordBuffer->Data(), bsize);
			trComp->TexCoordBuffer->unmap();
			trComp->GInstance["texcoord_buffer"]->setBuffer(trComp->TexCoordBuffer);
			if (attributes == "NoAttrib")
			{
				attributes = "Texcoord";
			}
			else
			{
				attributes += "Texcoord";
			}
		}
		else
		{
			trComp->TexCoordBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT2, 0);
			trComp->GInstance["texcoord_buffer"]->setBuffer(trComp->TexCoordBuffer);
		}
		if (attributes == "NoAttrib")
		{
			trComp->GInstance["solid_color"]->setFloat(optix::make_float3(0.5f, 0.5f, 0.5f));
		}
		/// select the appropriate programs
		std::string intersect_ptx	= GetResourceFilepath(intersect, attributes);
		std::string hitgroup_ptx	= GetResourceFilepath(intersect + "Hitgroup", attributes);
		std::string intersection	= GetResourceProgName(intersect + "Intersection", attributes);
		std::string bounding		= GetResourceProgName(intersect + "BoundingBox", attributes);
		std::string closest			= GetResourceProgName(intersect + "ClosestHit", attributes);
		std::string any				= GetResourceProgName(intersect + "AnyHit", attributes);

		trComp->IntersectionProg	= ctxComp->Context->createProgramFromPTXFile(intersect_ptx, intersection);
		trComp->BoundingBoxProgram	= ctxComp->Context->createProgramFromPTXFile(intersect_ptx, bounding);
		trComp->ClosestHitProgram	= ctxComp->Context->createProgramFromPTXFile(hitgroup_ptx, closest);
		trComp->AnyHitProgram		= ctxComp->Context->createProgramFromPTXFile(hitgroup_ptx, any);

		trComp->Geometry->setIntersectionProgram(trComp->IntersectionProg);
		trComp->Geometry->setBoundingBoxProgram(trComp->BoundingBoxProgram);
		trComp->Geometry->setPrimitiveCount(data->TIndexBuffer->Count());
		trComp->Material->setClosestHitProgram(0, trComp->ClosestHitProgram);
		trComp->Material->setAnyHitProgram(0, trComp->AnyHitProgram);
		trComp->GInstance->setGeometry(trComp->Geometry);
		trComp->GInstance->setMaterialCount(1);
		trComp->GInstance->setMaterial(0, trComp->Material);
		trComp->GGroup->addChild(trComp->GInstance);
		trComp->Transform->setChild(trComp->GGroup);
		trComp->GGroup->setAcceleration(trComp->Acceleration);
	}

	static void AttachTriangleMeshToTopLevelAcceleration(TriangleMeshComp& trComp, AccelerationComp& accelComp)
	{
		accelComp->Group->addChild(trComp->GGroup);
		accelComp->Acceleration->markDirty();
	}

	static void DetachTriangleMeshToTopLevelAcceleration(TriangleMeshComp& trComp, AccelerationComp& accelComp)
	{
		accelComp->Group->removeChild(trComp->GGroup);
		accelComp->Acceleration->markDirty();
	}

	static void SetPointcloudCompPointSize(PointCloudComp& pcComp, float size)
	{
		pcComp->GInstance["radius"]->setFloat(size);
	}
private:
	static constexpr const char* k_TriangleMeshPTxFilepath			= "FusionLib/Resources/Programs/TriangleMesh.ptx";
	static constexpr const char* k_TriangleMeshIntersectionProgName = "triangle_mesh_intersect";
	static constexpr const char* k_TriangleMeshBbboxProgName		= "triangle_mesh_bounds";
	static constexpr const char* k_SolidColorPtxFilepath			= "FusionLib/Resources/Programs/SolidColorHitGroup.ptx";
	static constexpr const char* k_TriangleMeshSolidColorClosestHitProgName = "solid_color_closest_hit";
	static constexpr const char* k_TriangleMeshSolidColorAnyHitProgName = "solid_color_any_hit";
	static constexpr const char* k_TriangleMeshClosestHitProgName	= "phong_closest_hit";
	static constexpr const char* k_TriangleMeshAnyHitProgName		= "phong_any_hit";
	static constexpr const char* k_PointCloudPtxFilepath 			= "FusionLib/Resources/Programs/PointCloud.ptx";
	static constexpr const char* k_PointCloudIntersectionProgName	= "pointcloud_intersect";
	static constexpr const char* k_PointCloudBboxProgName			= "pointcloud_bounds";
	static constexpr const char* k_PointCloudClosestHitProgName		= "pointcloud_closest_hit";
	static constexpr const char* k_PointCloudAnyHitProgName 		= "pointcloud_any_hit";
};	///	!class MeshMappingSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__