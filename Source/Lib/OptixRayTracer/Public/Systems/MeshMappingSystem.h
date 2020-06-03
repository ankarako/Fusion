#ifndef	__OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__

#include <Components/ContextComp.h>
#include <Components/RaygenProgComp.h>
#include <Components/TriangleMeshComp.h>
#include <Components/MeshMaterialComp.h>
#include <Components/AccelerationComp.h>
#include <Components/PointCloudComp.h>
#include <Components/ViewportFrustrumComp.h>
#include <Components/QuadComp.h>
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
		accelComp->Group->addChild(trComp->Transform);
		accelComp->Acceleration->markDirty();
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
			pcComp->Geometry["render_normals"]->setInt(0);
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
		trComp->Transform		= ctxComp->Context->createTransform();
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
		trComp->TransMat = optix::Matrix4x4::identity();
		trComp->Transform->setChild(trComp->GGroup);
		trComp->Transform->setMatrix(false, trComp->TransMat.getData(), nullptr);
		trComp->GGroup->setAcceleration(trComp->Acceleration);
	}

	void static MapMeshDataToTexturedMesh(const io::MeshData& data, TriangleMeshComp& trComp, ContextComp& ctxComp, const BufferCPU<uchar4>& texBuf, uint2 resolution)
	{
		trComp->Geometry = ctxComp->Context->createGeometry();
		trComp->GInstance = ctxComp->Context->createGeometryInstance();
		trComp->Material = ctxComp->Context->createMaterial();
		trComp->GGroup = ctxComp->Context->createGeometryGroup();
		trComp->Acceleration = ctxComp->Context->createAcceleration("Trbvh");
		trComp->Transform = ctxComp->Context->createTransform();
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
		std::string intersect_ptx = GetResourceFilepath(intersect, attributes);
		std::string hitgroup_ptx = GetResourceFilepath(intersect + "Hitgroup", attributes);
		std::string intersection = GetResourceProgName(intersect + "Intersection", attributes);
		std::string bounding = GetResourceProgName(intersect + "BoundingBox", attributes);
		//std::string closest = GetResourceProgName(intersect + "ClosestHit", attributes);
		//std::string any = GetResourceProgName(intersect + "AnyHit", attributes);

		trComp->IntersectionProg = ctxComp->Context->createProgramFromPTXFile(intersect_ptx, intersection);
		trComp->BoundingBoxProgram = ctxComp->Context->createProgramFromPTXFile(intersect_ptx, bounding);
		trComp->ClosestHitProgram = ctxComp->Context->createProgramFromPTXFile(k_TexturedMeshMaterialPtxFilepath, k_TexturedMeshMaterialClosetstHitProgName);
		trComp->AnyHitProgram = ctxComp->Context->createProgramFromPTXFile(k_TexturedMeshMaterialPtxFilepath, k_TexturedMeshMaterialAnyHitProgName);

		trComp->Geometry->setIntersectionProgram(trComp->IntersectionProg);
		trComp->Geometry->setBoundingBoxProgram(trComp->BoundingBoxProgram);
		trComp->Geometry->setPrimitiveCount(data->TIndexBuffer->Count());
		trComp->Material->setClosestHitProgram(0, trComp->ClosestHitProgram);
		trComp->Material->setAnyHitProgram(0, trComp->AnyHitProgram);
		trComp->GInstance->setGeometry(trComp->Geometry);
		trComp->GInstance->setMaterialCount(1);
		trComp->GInstance->setMaterial(0, trComp->Material);
		trComp->GGroup->addChild(trComp->GInstance);
		trComp->TransMat = optix::Matrix4x4::identity();
		trComp->Transform->setChild(trComp->GGroup);
		trComp->Transform->setMatrix(false, trComp->TransMat.getData(), nullptr);
		trComp->GGroup->setAcceleration(trComp->Acceleration);

		trComp->TextureBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE4, resolution.x, resolution.y);
		trComp->TextureSampler = ctxComp->Context->createTextureSampler();
		trComp->TextureSampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_LINEAR);
		trComp->TextureSampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
		trComp->TextureSampler->setWrapMode(0, RT_WRAP_REPEAT);
		trComp->TextureSampler->setWrapMode(1, RT_WRAP_REPEAT);
		trComp->TextureSampler->setWrapMode(2, RT_WRAP_REPEAT);
		trComp->TextureSampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
		trComp->TextureSampler->setMaxAnisotropy(1.0f);
		trComp->TextureSampler->setMipLevelCount(1u);
		trComp->TextureSampler->setArraySize(1u);
		trComp->TextureSampler->setBuffer(0u, 0u, trComp->TextureBuffer);
		trComp->GInstance["TextureBuffer"]->setBuffer(trComp->TextureBuffer);
		trComp->GInstance["TextureSampler"]->set(trComp->TextureSampler);
		uchar4* textureBufferData = (uchar4*)trComp->TextureBuffer->map();
		std::memcpy(textureBufferData, texBuf->Data(), texBuf->ByteSize());
		trComp->TextureBuffer->unmap();
	}

	void static MapMeshDataToPerfcapTexturedMesh(const io::MeshData& data, TriangleMeshComp& trComp, ContextComp& ctxComp, const BufferCPU<uchar4>& texBuf, uint2 resolution)
	{
		trComp->Geometry = ctxComp->Context->createGeometry();
		trComp->GInstance = ctxComp->Context->createGeometryInstance();
		trComp->Material = ctxComp->Context->createMaterial();
		trComp->GGroup = ctxComp->Context->createGeometryGroup();
		trComp->Acceleration = ctxComp->Context->createAcceleration("Trbvh");
		trComp->Transform = ctxComp->Context->createTransform();
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
		std::string intersect_ptx = k_PerfcapTriangleMeshPtxFilepath;
		std::string hitgroup_ptx = GetResourceFilepath(intersect + "Hitgroup", attributes);
		std::string intersection = k_PerfcapTriangleMeshIntersectionProgName;
		std::string bounding = k_PerfcapTriangleMeshBbboxProgName;
		//std::string closest = GetResourceProgName(intersect + "ClosestHit", attributes);
		//std::string any = GetResourceProgName(intersect + "AnyHit", attributes);

		trComp->IntersectionProg = ctxComp->Context->createProgramFromPTXFile(intersect_ptx, intersection);
		trComp->BoundingBoxProgram = ctxComp->Context->createProgramFromPTXFile(intersect_ptx, bounding);
		trComp->ClosestHitProgram = ctxComp->Context->createProgramFromPTXFile(k_TexturedMeshMaterialPtxFilepath, k_TexturedMeshMaterialClosetstHitProgName);
		trComp->AnyHitProgram = ctxComp->Context->createProgramFromPTXFile(k_TexturedMeshMaterialPtxFilepath, k_TexturedMeshMaterialAnyHitProgName);

		trComp->Geometry->setIntersectionProgram(trComp->IntersectionProg);
		trComp->Geometry->setBoundingBoxProgram(trComp->BoundingBoxProgram);
		trComp->Geometry->setPrimitiveCount(data->TIndexBuffer->Count());
		trComp->Material->setClosestHitProgram(0, trComp->ClosestHitProgram);
		trComp->Material->setAnyHitProgram(0, trComp->AnyHitProgram);
		trComp->GInstance->setGeometry(trComp->Geometry);
		trComp->GInstance->setMaterialCount(1);
		trComp->GInstance->setMaterial(0, trComp->Material);
		trComp->GGroup->addChild(trComp->GInstance);
		trComp->TransMat = optix::Matrix4x4::identity();
		trComp->Transform->setChild(trComp->GGroup);
		trComp->Transform->setMatrix(false, trComp->TransMat.getData(), nullptr);
		trComp->GGroup->setAcceleration(trComp->Acceleration);

		trComp->TextureBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE4, resolution.x, resolution.y);
		trComp->TextureSampler = ctxComp->Context->createTextureSampler();
		trComp->TextureSampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_LINEAR);
		trComp->TextureSampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
		trComp->TextureSampler->setWrapMode(0, RT_WRAP_REPEAT);
		trComp->TextureSampler->setWrapMode(1, RT_WRAP_REPEAT);
		trComp->TextureSampler->setWrapMode(2, RT_WRAP_REPEAT);
		trComp->TextureSampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
		trComp->TextureSampler->setMaxAnisotropy(1.0f);
		trComp->TextureSampler->setMipLevelCount(1u);
		trComp->TextureSampler->setArraySize(1u);
		trComp->TextureSampler->setBuffer(0u, 0u, trComp->TextureBuffer);
		trComp->GInstance["TextureBuffer"]->setBuffer(trComp->TextureBuffer);
		trComp->GInstance["TextureSampler"]->set(trComp->TextureSampler);
		uchar4* textureBufferData = (uchar4*)trComp->TextureBuffer->map();
		std::memcpy(textureBufferData, texBuf->Data(), texBuf->ByteSize());
		trComp->TextureBuffer->unmap();
	}

	static void MapMeshDataToTexturingTriangleMesh(const io::MeshData& data, TriangleMeshComp& trComp, ContextComp& ctxComp)
	{
		trComp->Geometry = ctxComp->Context->createGeometry();
		trComp->GInstance = ctxComp->Context->createGeometryInstance();
		trComp->Material = ctxComp->Context->createMaterial();
		trComp->GGroup = ctxComp->Context->createGeometryGroup();
		trComp->Acceleration = ctxComp->Context->createAcceleration("Trbvh");
		trComp->Transform = ctxComp->Context->createTransform();

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
		{
			int count = data->NormalBuffer->Count();
			int bsize = data->NormalBuffer->ByteSize();
			trComp->NormalBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, count);
			std::memcpy(trComp->NormalBuffer->map(), data->NormalBuffer->Data(), bsize);
			trComp->NormalBuffer->unmap();
			trComp->GInstance["normal_buffer"]->setBuffer(trComp->NormalBuffer);
		}
		{
			int count = data->TexcoordBuffer->Count();
			int bsize = data->TexcoordBuffer->ByteSize();
			trComp->TexCoordBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT2, count);
			std::memcpy(trComp->TexCoordBuffer->map(), data->TexcoordBuffer->Data(), bsize);
			trComp->TexCoordBuffer->unmap();
			trComp->GInstance["texcoord_buffer"]->setBuffer(trComp->TexCoordBuffer);
		}
		{
			/// no colors
			trComp->ColorsBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE4, 0);
			trComp->GInstance["color_buffer"]->setBuffer(trComp->ColorsBuffer);
		}
		std::string intersect_ptx = rt::GetResourceFilepath("TriangleMesh", "NormalsColorTexcoord");
		std::string intersect_name = rt::GetResourceProgName("TriangleMeshIntersection", "NormalsColorTexcoord");
		std::string bounds_name = rt::GetResourceProgName("TriangleMeshBoundingBox", "NormalsColorTexcoord");

		trComp->IntersectionProg = ctxComp->Context->createProgramFromPTXFile(intersect_ptx, intersect_name);
		trComp->BoundingBoxProgram = ctxComp->Context->createProgramFromPTXFile(intersect_ptx, bounds_name);
		trComp->ClosestHitProgram = ctxComp->Context->createProgramFromPTXFile(k_TexturingMeshHitGroupPtxFilepath, k_TexturingMeshClosestHitProgName);
		trComp->AnyHitProgram = ctxComp->Context->createProgramFromPTXFile(k_TexturingMeshHitGroupPtxFilepath, k_TexturingMeshAnyHitProgName);

		trComp->Geometry->setPrimitiveCount(data->TIndexBuffer->Count());
		trComp->Geometry->setIntersectionProgram(trComp->IntersectionProg);
		trComp->Geometry->setBoundingBoxProgram(trComp->BoundingBoxProgram);
		trComp->Material->setClosestHitProgram(0, trComp->ClosestHitProgram);
		trComp->Material->setAnyHitProgram(0, trComp->AnyHitProgram);

		trComp->GInstance->setGeometry(trComp->Geometry);
		trComp->GInstance->setMaterialCount(1);
		trComp->GInstance->setMaterial(0, trComp->Material);
		trComp->GGroup->addChild(trComp->GInstance);
		trComp->TransMat = optix::Matrix4x4::identity();
		trComp->Transform->setMatrix(false, trComp->TransMat.getData(), nullptr);
		trComp->GGroup->setAcceleration(trComp->Acceleration);
		trComp->Transform->setChild(trComp->GGroup);
	}

	static void MapMeshDataToPerfcapTriangleMesh(const io::MeshData& data, TriangleMeshComp& trComp, ContextComp& ctxComp)
	{
		trComp->Geometry = ctxComp->Context->createGeometry();
		trComp->GInstance = ctxComp->Context->createGeometryInstance();
		trComp->Material = ctxComp->Context->createMaterial();
		trComp->GGroup = ctxComp->Context->createGeometryGroup();
		trComp->Acceleration = ctxComp->Context->createAcceleration("Trbvh");
		trComp->Transform = ctxComp->Context->createTransform();

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
		{
			int count = data->NormalBuffer->Count();
			int bsize = data->NormalBuffer->ByteSize();
			trComp->NormalBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, count);
			std::memcpy(trComp->NormalBuffer->map(), data->NormalBuffer->Data(), bsize);
			trComp->NormalBuffer->unmap();
			trComp->GInstance["normal_buffer"]->setBuffer(trComp->NormalBuffer);
		}
		{
			int count = data->TexcoordBuffer->Count();
			int bsize = data->TexcoordBuffer->ByteSize();
			trComp->TexCoordBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT2, count);
			std::memcpy(trComp->TexCoordBuffer->map(), data->TexcoordBuffer->Data(), bsize);
			trComp->TexCoordBuffer->unmap();
			trComp->GInstance["texcoord_buffer"]->setBuffer(trComp->TexCoordBuffer);
		}
		{
			/// no colors
			trComp->ColorsBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE4, 0);
			trComp->GInstance["color_buffer"]->setBuffer(trComp->ColorsBuffer);
		}
		std::string intersect_ptx = k_PerfcapTriangleMeshPtxFilepath;
		std::string intersect_name = k_PerfcapTriangleMeshIntersectionProgName;
		std::string bounds_name = k_PerfcapTriangleMeshBbboxProgName;

		trComp->IntersectionProg = ctxComp->Context->createProgramFromPTXFile(intersect_ptx, intersect_name);
		trComp->BoundingBoxProgram = ctxComp->Context->createProgramFromPTXFile(intersect_ptx, bounds_name);
		trComp->ClosestHitProgram = ctxComp->Context->createProgramFromPTXFile(k_TexturingMeshHitGroupPtxFilepath, k_TexturingMeshClosestHitProgName);
		trComp->AnyHitProgram = ctxComp->Context->createProgramFromPTXFile(k_TexturingMeshHitGroupPtxFilepath, k_TexturingMeshAnyHitProgName);

		trComp->Geometry->setPrimitiveCount(data->TIndexBuffer->Count());
		trComp->Geometry->setIntersectionProgram(trComp->IntersectionProg);
		trComp->Geometry->setBoundingBoxProgram(trComp->BoundingBoxProgram);
		trComp->Material->setClosestHitProgram(0, trComp->ClosestHitProgram);
		trComp->Material->setAnyHitProgram(0, trComp->AnyHitProgram);

		trComp->GInstance->setGeometry(trComp->Geometry);
		trComp->GInstance->setMaterialCount(1);
		trComp->GInstance->setMaterial(0, trComp->Material);
		trComp->GGroup->addChild(trComp->GInstance);
		trComp->TransMat = optix::Matrix4x4::identity();
		trComp->Transform->setMatrix(false, trComp->TransMat.getData(), nullptr);
		trComp->GGroup->setAcceleration(trComp->Acceleration);
		trComp->Transform->setChild(trComp->GGroup);
	}

	static void AttachTriangleMeshToTopLevelAcceleration(TriangleMeshComp& trComp, AccelerationComp& accelComp)
	{
		accelComp->Group->addChild(trComp->Transform);
		//accelComp->Transform->setChild(trComp->Transform);
		accelComp->Acceleration->markDirty();
	}

	static void MapTextureToTexturedMesh(TriangleMeshComp& trComp, const BufferCPU<uchar4>& tex)
	{
		std::memcpy(trComp->TextureBuffer->map(), tex->Data(), tex->ByteSize());
		trComp->TextureBuffer->unmap();
	}

	static void DetachTriangleMeshToTopLevelAcceleration(TriangleMeshComp& trComp, AccelerationComp& accelComp)
	{
		accelComp->Group->removeChild(trComp->Transform);
		accelComp->Acceleration->markDirty();
	}

	static void SetPointcloudCompPointSize(PointCloudComp& pcComp, float size)
	{
		pcComp->GInstance["radius"]->setFloat(size);
	}

	static void SetTriangleMeshComponentTranslation(TriangleMeshComp& trComp, float x, float y, float z)
	{
		/*optix::Matrix4x4 trans;
		trComp->Transform->getMatrix(false, trans.getData(), nullptr);*/
		trComp->TransMat *= trComp->TransMat.translate(make_float3(x, -y, z));
		trComp->Transform->setMatrix(false, trComp->TransMat.getData(), nullptr);
		trComp->Acceleration->markDirty();
	}

	static void SetTriangleMeshComponentRotation(TriangleMeshComp& trComp, float x_rot, float y_rot, float z_rot)
	{
		optix::Matrix4x4 mat;
		trComp->Transform->getMatrix(false, mat.getData(), nullptr);
		optix::float3 axis = optix::normalize(optix::make_float3(x_rot, y_rot, z_rot));
		float xrad = x_rot * M_PIf / 180.0f;
		float yrad = y_rot * M_PIf / 180.0f;
		float zrad = z_rot * M_PIf / 180.0f;
		float rot;
		
		if (xrad != 0.0f)
		{
			rot = xrad;
		}
		else if (yrad != 0.0f)
		{
			rot = -yrad;
		}
		else if (zrad != 0.0f)
		{
			rot = zrad;
		}
		trComp->TransMat *= trComp->TransMat.rotate(rot, axis);
		trComp->Transform->setMatrix(false, trComp->TransMat.getData(), nullptr);
		trComp->Acceleration->markDirty();
	}

	static void SetTriangleMeshComponentScale(TriangleMeshComp& trComp, float scale)
	{
		optix::Matrix4x4 mat;
		trComp->Transform->getMatrix(false, mat.getData(), nullptr);
		trComp->TransMat *= trComp->TransMat.scale(optix::make_float3(scale, scale, scale));
		trComp->Transform->setMatrix(false, trComp->TransMat.getData(), nullptr);
		trComp->Acceleration->markDirty();
	}

	static void AttachNormalsToPointcloudComponent(PointCloudComp& pcComp, ContextComp& ctxComp, const BufferCPU<float3>& normalsBuffer)
	{
		unsigned int count = normalsBuffer->Count();
		unsigned int bsize = normalsBuffer->ByteSize();
		pcComp->NormalBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, count);
		std::memcpy(pcComp->NormalBuffer->map(), normalsBuffer->Data(), bsize);
	}

	static void RenderPointCloudNormals(PointCloudComp& pcComp, bool render)
	{
		if (render)
		{
			pcComp->Geometry["render_normals"]->setInt(1);
		}
		else
		{
			pcComp->Geometry["render_normals"]->setInt(0);
		}
	}
	///	\brief map a viewport frustrum component
	///	\param	viewComp	the viewportFrustrumComp to map
	///	\param	ctxComp		the context component associated with the viewport frustrum
	///	\param	trans		the transformation matrix of the frustrum
	///	\param	plane
	///	\param	v1
	///	\param	v2
	///	\param	anchor
	///	\param	
	static void MapViewportFrustrumComp(ViewportFrustrumComp& viewComp, ContextComp& ctxComp)
	{
		viewComp->Geometry 				= ctxComp->Context->createGeometry();
		viewComp->Material 				= ctxComp->Context->createMaterial();
		viewComp->GInstance 			= ctxComp->Context->createGeometryInstance();
		viewComp->GGroup 				= ctxComp->Context->createGeometryGroup();
		viewComp->Acceleration 			= ctxComp->Context->createAcceleration("Trbvh");
		viewComp->Transform 			= ctxComp->Context->createTransform();
		viewComp->IntersectionProgram 	= ctxComp->Context->createProgramFromPTXFile(k_ParallelogramPtxFilepath, k_ParallelogramIntersectionProgName);
		viewComp->BoundingBoxProgram 	= ctxComp->Context->createProgramFromPTXFile(k_ParallelogramPtxFilepath, k_ParallelogramBoundingBoxProgName);
		/// for now we will place a solid color hitgroup
		viewComp->ClosestHitProgram		= ctxComp->Context->createProgramFromPTXFile(k_SolidColorPtxFilepath, k_TriangleMeshSolidColorClosestHitProgName);
		viewComp->AnyhitProgram			= ctxComp->Context->createProgramFromPTXFile(k_SolidColorPtxFilepath, k_TriangleMeshSolidColorAnyHitProgName);
		///
		viewComp->GInstance["solid_color"]->setFloat(optix::make_float3(1.0f, 0.0f, 0.0f));
		optix::float3 anchor = optix::make_float3(-13.0f, 0.0f, -25.0f);	
		viewComp->V1 = optix::make_float3(-50.0f, 0.0f, -50.0f);
		viewComp->V2			= optix::make_float3(50.0f, 0.0f, 50.0f );
		optix::float3 normal	= optix::cross(viewComp->V1, viewComp->V2);
		normal 					= optix::normalize(normal);
		float d = optix::dot(normal, anchor);
		viewComp->V1 *= 1.0f / optix::dot(viewComp->V1, viewComp->V1);
		viewComp->V2 *= 1.0f / optix::dot(viewComp->V2, viewComp->V2);
		optix::float4 plane = optix::make_float4(normal, d);
		viewComp->GInstance["plane"]->setFloat(plane);
		viewComp->GInstance["v1"]->setFloat(viewComp->V1);
		viewComp->GInstance["v2"]->setFloat(viewComp->V2);
		viewComp->GInstance["anchor"]->setFloat(anchor);
		viewComp->Geometry->setBoundingBoxProgram(viewComp->BoundingBoxProgram);
		viewComp->Geometry->setIntersectionProgram(viewComp->IntersectionProgram);
		viewComp->Geometry->setPrimitiveCount(1u);
		viewComp->Material->setClosestHitProgram(0, viewComp->ClosestHitProgram);
		viewComp->Material->setAnyHitProgram(0, viewComp->AnyhitProgram);
		viewComp->GInstance->setGeometry(viewComp->Geometry);
		viewComp->GInstance->setMaterialCount(1);
		viewComp->GInstance->setMaterial(0, viewComp->Material);
		viewComp->GGroup->setChildCount(1);
		viewComp->GGroup->setChild(0, viewComp->GInstance);
		viewComp->TransMat = optix::Matrix4x4::identity();
		viewComp->Transform->setChild(viewComp->GGroup);
		viewComp->Transform->setMatrix(false, viewComp->TransMat.getData(), nullptr);
		viewComp->GGroup->setAcceleration(viewComp->Acceleration);
	}

	static void AttachViewportCompToToLevelAcceleration(ViewportFrustrumComp& viewComp, AccelerationComp& acceleration)
	{
		acceleration->Group->addChild(viewComp->Transform);
		acceleration->Acceleration->markDirty();
	}

	static void MapQuadComp(QuadComp& qComp, ContextComp& ctxComp)
	{
		/// create quad's vertices
		float w = qComp->Width;
		float h = qComp->Height;
		optix::float3 a = qComp->Anchor;
		optix::float3 v0 = optix::make_float3(a.x - w * 0.5f, a.y + h * 0.5f, a.z);
		optix::float3 v1 = optix::make_float3(a.x + w * 0.5f, a.y + h * 0.5f, a.z);
		optix::float3 v2 = optix::make_float3(a.x - w * 0.5f, a.y - h * 0.5f, a.z);
		optix::float3 v3 = optix::make_float3(a.x + w * 0.5f, a.y - h * 0.5f, a.z);
		BufferCPU<optix::float3> vBuf = CreateBufferCPU<optix::float3>(4);
		vBuf->Data()[0] = v0;
		vBuf->Data()[1] = v1;
		vBuf->Data()[2] = v2;
		vBuf->Data()[3] = v3;
		/// create vertex buffer
		qComp->VertexBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, 4);
		std::memcpy(qComp->VertexBuffer->map(), vBuf->Data(), 4 * sizeof(optix::float3));
		qComp->VertexBuffer->unmap();
		/// dummy color
		qComp->Color = optix::make_float3(1.0f, 1.0f, 1.0f);
		///
		qComp->Geometry = ctxComp->Context->createGeometry();
		qComp->Material = ctxComp->Context->createMaterial();
		qComp->GInstance = ctxComp->Context->createGeometryInstance();
		qComp->GGroup = ctxComp->Context->createGeometryGroup();
		qComp->Acceleration = ctxComp->Context->createAcceleration("Sbvh");
		qComp->Transform = ctxComp->Context->createTransform();
		qComp->TransMat = optix::Matrix4x4::identity();
		try 
		{
			qComp->IntersectionProgram = ctxComp->Context->createProgramFromPTXFile(k_QuadPtxFilepath, k_QuadIntersectionProgName);
			qComp->BoundingBoxProgram = ctxComp->Context->createProgramFromPTXFile(k_QuadPtxFilepath, k_QuadBoundingBoxProgName);
			qComp->ClosestHitProgram = ctxComp->Context->createProgramFromPTXFile(k_SolidColorPtxFilepath, k_TriangleMeshSolidColorClosestHitProgName);
			qComp->AnyHitProgram = ctxComp->Context->createProgramFromPTXFile(k_SolidColorPtxFilepath, k_TriangleMeshSolidColorAnyHitProgName);
		}
		catch (optix::Exception& ex)
		{
			LOG_ERROR << ex.what();
		}
		qComp->Geometry->setIntersectionProgram(qComp->IntersectionProgram);
		qComp->Geometry->setBoundingBoxProgram(qComp->BoundingBoxProgram);
		qComp->Geometry->setPrimitiveCount(1);
		qComp->Material->setClosestHitProgram(0, qComp->ClosestHitProgram);
		qComp->Material->setAnyHitProgram(0, qComp->AnyHitProgram);
		qComp->GInstance->setGeometry(qComp->Geometry);
		qComp->GInstance->setMaterialCount(1);
		qComp->GInstance->setMaterial(0, qComp->Material);
		qComp->GGroup->setChildCount(1);
		qComp->GGroup->setChild(0, qComp->GInstance);
		qComp->GGroup->setAcceleration(qComp->Acceleration);
		qComp->Transform->setChild(qComp->GGroup);
		qComp->Transform->setMatrix(false, qComp->TransMat.getData(), nullptr);
		qComp->GInstance["vertex_buffer"]->setBuffer(qComp->VertexBuffer);
		qComp->GInstance["solid_color"]->setFloat(qComp->Color);
	}

	static void CopyAnimatedMeshDataToTriangleComp(TriangleMeshComp& trComp, const io::MeshData& data)
	{
		std::memcpy(trComp->VertexBuffer->map(), data->VertexBuffer->Data(), data->VertexBuffer->ByteSize());
		trComp->VertexBuffer->unmap();
		trComp->Acceleration->markDirty();
	}

	static void AttachQuadCompToTopLevelAcceleration(QuadComp& qComp, AccelerationComp& accComp)
	{
		accComp->Group->addChild(qComp->Transform);
		accComp->Acceleration->markDirty();
	}

	static void QuadCompSetTransMat(QuadComp& qComp, const optix::Matrix4x4& transMat)
	{
		qComp->TransMat = transMat;
		qComp->Transform->setMatrix(false, qComp->TransMat.getData(), nullptr);
		qComp->Acceleration->markDirty();
	}
private:
	static constexpr const char* k_TriangleMeshPTxFilepath					= "FusionLib/Resources/Programs/TriangleMesh.ptx";
	static constexpr const char* k_TriangleMeshIntersectionProgName 		= "triangle_mesh_intersect";
	static constexpr const char* k_TriangleMeshBbboxProgName				= "triangle_mesh_bounds";
	static constexpr const char* k_SolidColorPtxFilepath					= "FusionLib/Resources/Programs/SolidColorHitGroup.ptx";
	static constexpr const char* k_TriangleMeshSolidColorClosestHitProgName = "solid_color_closest_hit";
	static constexpr const char* k_TriangleMeshSolidColorAnyHitProgName 	= "solid_color_any_hit";
	static constexpr const char* k_TriangleMeshClosestHitProgName			= "phong_closest_hit";
	static constexpr const char* k_TriangleMeshAnyHitProgName				= "phong_any_hit";
	static constexpr const char* k_PointCloudPtxFilepath 					= "FusionLib/Resources/Programs/PointCloud.ptx";
	static constexpr const char* k_PointCloudIntersectionProgName			= "pointcloud_intersect";
	static constexpr const char* k_PointCloudBboxProgName					= "pointcloud_bounds";
	static constexpr const char* k_PointCloudClosestHitProgName				= "pointcloud_closest_hit";
	static constexpr const char* k_PointCloudAnyHitProgName 				= "pointcloud_any_hit";
	static constexpr const char* k_ParallelogramPtxFilepath 				= "FusionLib/Resources/Programs/Parallelogram.ptx";
	static constexpr const char* k_ParallelogramIntersectionProgName		= "parallelogram_intersect";
	static constexpr const char* k_ParallelogramBoundingBoxProgName			= "parallelogram_bounds";
	static constexpr const char* k_QuadPtxFilepath 							= "FusionLib/Resources/Programs/Quad.ptx";
	static constexpr const char* k_QuadIntersectionProgName					= "quad_intersect";
	static constexpr const char* k_QuadBoundingBoxProgName					= "quad_bounds";
	static constexpr const char* k_TexturedMeshMaterialPtxFilepath			= "FusionLib/Resources/Programs/TexturedMeshMaterial.ptx";
	static constexpr const char* k_TexturedMeshMaterialClosetstHitProgName	= "closest_hit_radiance";
	static constexpr const char* k_TexturedMeshMaterialAnyHitProgName		= "any_hit";
	static constexpr const char* k_TexturingMeshHitGroupPtxFilepath			= "FusionLib/Resources/Programs/TriangleTexturingHitGroup.ptx";
	static constexpr const char* k_TexturingMeshClosestHitProgName			= "closest_hit_texturing";
	static constexpr const char* k_TexturingMeshAnyHitProgName				= "any_hit";
	static constexpr const char* k_PerfcapTriangleMeshPtxFilepath			= "FusionLib/Resources/Programs/PerfcapTriangleMesh.ptx";
	static constexpr const char* k_PerfcapTriangleMeshIntersectionProgName	= "triangle_mesh_intersect";
	static constexpr const char* k_PerfcapTriangleMeshBbboxProgName			= "triangle_mesh_bounds";
};	///	!class MeshMappingSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_MESHMAPPINGSYSTEM_H__