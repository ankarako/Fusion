#ifndef __IO_PUBLIC_LOADOBJ_H__
#define __IO_PUBLIC_LOADOBJ_H__

#include <MeshData.h>
#include <PerfcapAnimationData.h>
#include <plog/Log.h>
#include <tiny_obj_loader.h>
#include <fstream>
#include <string>

namespace fu {
namespace io {

static void LoadObj(const std::string& filepath, io::MeshData& data)
{
	tinyobj::attrib_t					attrib;
	std::vector<tinyobj::shape_t>		shapes;
	std::vector<tinyobj::material_t>	materials;
	std::string warn;
	std::string err;

	std::ifstream infd(filepath);
	if (!infd.good())
	{
		LOG_ERROR << "Failed to open file: " << filepath;
		MeshData{};
	}

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &infd);
	infd.close();
	if (!warn.empty())
	{
		LOG_WARNING << warn;
	}
	if (!err.empty())
	{
		LOG_ERROR << err;
	}
	if (!ret)
	{
		LOG_ERROR << "Failed to open file: " << filepath;
		return;
	}
	LOG_DEBUG << "[Obj Loader]";
	LOG_DEBUG << "Parsed file: " << filepath;
	LOG_DEBUG << "[attributes]";
	LOG_DEBUG << "Number of vertices : " << (int)(attrib.vertices.size() / 3);
	LOG_DEBUG << "Number of normals  : " << (int)(attrib.normals.size() / 3);
	LOG_DEBUG << "Number of colors   : " << (int)(attrib.colors.size() / 3);
	LOG_DEBUG << "Number of texcoords: " << (int)(attrib.texcoords.size() / 2);
	LOG_DEBUG << "Number of materials: " << (int)(materials.size());
	LOG_DEBUG << "Number of shapes   : " << (int)(shapes.size());
	for (int i = 0; i < shapes.size(); i++)
	{
		LOG_DEBUG << "Shape #" << i << ": ";
		LOG_DEBUG << "\tIndices: " << shapes[i].mesh.indices.size() / 3;
	}
	if (shapes.size() > 1)
	{
		LOG_ERROR << "Currently single shape loading is supported";
		return;
	}
	std::vector<int> indices;
	std::vector<float3> vertices;
	std::vector<float2> texcoords;
	std::vector<float3> normals;
	tinyobj::shape_t shape = shapes[0];
	int tid = 0;
	for (int v = 0; v < attrib.vertices.size(); ++v)
	{
		float3 vertex = make_float3(
			attrib.vertices[3 * v + 0],
			attrib.vertices[3 * v + 1],
			attrib.vertices[3 * v + 2]
		);
		vertices.emplace_back(vertex);
	}
	for (const auto& index : shape.mesh.indices)
	{
		float2 texcoord = make_float2(
			attrib.texcoords[2 * index.texcoord_index + 0],
			attrib.texcoords[2 * index.texcoord_index + 1]
		);

		float3 normal = make_float3(
			attrib.normals[3 * index.normal_index + 0],
			attrib.normals[3 * index.normal_index + 1],
			attrib.normals[3 * index.normal_index + 2]
		);

		//vertices.emplace_back(vertex);
		normals.emplace_back(normal);
		texcoords.emplace_back(texcoord);
		indices.emplace_back(tid);
		tid++;
	}
	///
	//MeshData data = CreateMeshData();
	if (!vertices.empty())
	{
		int count = vertices.size();
		int bsize = vertices.size() * sizeof(float3);
		data->VertexBuffer = CreateBufferCPU<float3>(count);
		std::memcpy(data->VertexBuffer->Data(), vertices.data(), bsize);
	}
	if (!indices.empty())
	{
		data->HasFaces = true;
		int count = indices.size() / 3;
		int bsize = indices.size() * sizeof(unsigned int);
		data->TIndexBuffer = CreateBufferCPU<uint3>(count);
		std::memcpy(data->TIndexBuffer->Data(), indices.data(), bsize);
	}
	if (!normals.empty())
	{
		data->HasNormals = true;
		int count = normals.size();
		int bsize = normals.size() * sizeof(float3);
		data->NormalBuffer = CreateBufferCPU<float3>(count);
		std::memcpy(data->NormalBuffer->Data(), normals.data(), bsize);
	}
	// FIXME: Hack for colors
	data->HasColors = true;
	int count = attrib.vertices.size() / 3;
	data->ColorBuffer = CreateBufferCPU<uchar4>(count);
	int bsize = count * sizeof(uchar4);
	uchar4 color = make_uchar4(200, 200, 200, 255);
	for (int i = 0; i < count; ++i)
	{
		data->ColorBuffer->Data()[i] = color;
	}
	//if (!attrib.colors.empty())
	//{
	//	data->HasColors = true;
	//	int count = attrib.colors.size() / 3;
	//	int bsize = attrib.colors.size() * sizeof(float);
	//	/// convert colors to bytes
	//	data->ColorBuffer = CreateBufferCPU<uchar4>(count);
	//	std::memcpy(data->ColorBuffer->Data(), attrib.colors.data(), bsize);
	//}
	if (!texcoords.empty())
	{
		data->HasTexcoords = true;
		int count = texcoords.size();
		int bsize = texcoords.size() * sizeof(float2);
		data->TexcoordBuffer = CreateBufferCPU<float2>(count);
		std::memcpy(data->TexcoordBuffer->Data(), texcoords.data(), bsize);
	}
}

static void LoadObjWithSkinData(const std::string& filepath, io::MeshData& data, const io::perfcap_skin_data_ptr_t& skin_data, io::perfcap_skin_data_ptr_t& denseSkinData)
{
	tinyobj::attrib_t					attrib;
	std::vector<tinyobj::shape_t>		shapes;
	std::vector<tinyobj::material_t>	materials;
	std::string warn;
	std::string err;

	denseSkinData->NumWeightsPerVertex = skin_data->NumWeightsPerVertex;

	std::ifstream infd(filepath);
	if (!infd.good())
	{
		LOG_ERROR << "Failed to open file: " << filepath;
		MeshData{};
	}

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &infd);
	infd.close();
	if (!warn.empty())
	{
		LOG_WARNING << warn;
	}
	if (!err.empty())
	{
		LOG_ERROR << err;
	}
	if (!ret)
	{
		LOG_ERROR << "Failed to open file: " << filepath;
		return;
	}
	LOG_DEBUG << "[Obj Loader]";
	LOG_DEBUG << "Parsed file: " << filepath;
	LOG_DEBUG << "[attributes]";
	LOG_DEBUG << "Number of vertices : " << (int)(attrib.vertices.size() / 3);
	LOG_DEBUG << "Number of normals  : " << (int)(attrib.normals.size() / 3);
	LOG_DEBUG << "Number of colors   : " << (int)(attrib.colors.size() / 3);
	LOG_DEBUG << "Number of texcoords: " << (int)(attrib.texcoords.size() / 2);
	LOG_DEBUG << "Number of materials: " << (int)(materials.size());
	LOG_DEBUG << "Number of shapes   : " << (int)(shapes.size());
	for (int i = 0; i < shapes.size(); i++)
	{
		LOG_DEBUG << "Shape #" << i << ": ";
		LOG_DEBUG << "\tIndices: " << shapes[i].mesh.indices.size() / 3;
	}
	if (shapes.size() > 1)
	{
		LOG_ERROR << "Currently single shape loading is supported";
		return;
	}
	std::vector<int> indices;
	//std::vector<float3> vertices;
	std::vector<float2> texcoords;
	//std::vector<float3> normals;
	tinyobj::shape_t shape = shapes[0];
	int tid = 0;
	int numWeightsPerVertex = denseSkinData->NumWeightsPerVertex;
	
	data->VertexBuffer = CreateBufferCPU<float3>(attrib.vertices.size() / 3);
	data->NormalBuffer = CreateBufferCPU<float3>(attrib.normals.size() / 3);
	std::memcpy(data->VertexBuffer->Data(), attrib.vertices.data(), attrib.vertices.size() * sizeof(float));
	std::memcpy(data->NormalBuffer->Data(), attrib.normals.data(), attrib.normals.size() * sizeof(float));

	for (const auto& index : shape.mesh.indices)
	{
		//float3 vertex = make_float3(
		//	attrib.vertices[3 * index.vertex_index + 0],
		//	attrib.vertices[3 * index.vertex_index + 1],
		//	attrib.vertices[3 * index.vertex_index + 2]
		//);

		float2 texcoord = make_float2(
			attrib.texcoords[2 * index.texcoord_index + 0],
			attrib.texcoords[2 * index.texcoord_index + 1]
		);

		//float3 normal = make_float3(
		//	attrib.normals[3 * index.normal_index + 0],
		//	attrib.normals[3 * index.normal_index + 1],
		//	attrib.normals[3 * index.normal_index + 2]
		//);

		for (int w = 0; w < denseSkinData->NumWeightsPerVertex; ++w)
		{
			denseSkinData->WeightData.emplace_back(skin_data->WeightData[index.vertex_index * numWeightsPerVertex + w]);
			denseSkinData->Jointdata.emplace_back(skin_data->Jointdata[index.vertex_index * numWeightsPerVertex + w]);
		}

		//vertices.emplace_back(vertex);
		//normals.emplace_back(normal);
		texcoords.emplace_back(texcoord);
		indices.emplace_back(index.vertex_index);
		tid++;
	}
	denseSkinData->NumVertices = denseSkinData->WeightData.size() / numWeightsPerVertex;
	///
	//MeshData data = CreateMeshData();
	/*if (!vertices.empty())
	{
		int count = vertices.size();
		int bsize = vertices.size() * sizeof(float3);
		data->VertexBuffer = CreateBufferCPU<float3>(count);
		std::memcpy(data->VertexBuffer->Data(), vertices.data(), bsize);
	}*/
	if (!indices.empty())
	{
		data->HasFaces = true;
		int count = indices.size() / 3;
		int bsize = indices.size() * sizeof(unsigned int);
		data->TIndexBuffer = CreateBufferCPU<uint3>(count);
		std::memcpy(data->TIndexBuffer->Data(), indices.data(), bsize);
	}
	//if (!normals.empty())
	//{
	//	data->HasNormals = true;
	//	int count = normals.size();
	//	int bsize = normals.size() * sizeof(float3);
	//	data->NormalBuffer = CreateBufferCPU<float3>(count);
	//	std::memcpy(data->NormalBuffer->Data(), normals.data(), bsize);
	//}
	// FIXME: Hack for colors
	data->HasColors = true;
	int count = attrib.vertices.size() / 3;
	data->ColorBuffer = CreateBufferCPU<uchar4>(count);
	int bsize = count * sizeof(uchar4);
	uchar4 color = make_uchar4(200, 200, 200, 255);
	for (int i = 0; i < count; ++i)
	{
		data->ColorBuffer->Data()[i] = color;
	}
	//if (!attrib.colors.empty())
	//{
	//	data->HasColors = true;
	//	int count = attrib.colors.size() / 3;
	//	int bsize = attrib.colors.size() * sizeof(float);
	//	/// convert colors to bytes
	//	data->ColorBuffer = CreateBufferCPU<uchar4>(count);
	//	std::memcpy(data->ColorBuffer->Data(), attrib.colors.data(), bsize);
	//}
	if (!texcoords.empty())
	{
		data->HasTexcoords = true;
		int count = texcoords.size();
		int bsize = texcoords.size() * sizeof(float2);
		data->TexcoordBuffer = CreateBufferCPU<float2>(count);
		std::memcpy(data->TexcoordBuffer->Data(), texcoords.data(), bsize);
	}
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_LOADOBJ_H__