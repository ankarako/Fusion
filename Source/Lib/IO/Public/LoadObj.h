#ifndef __IO_PUBLIC_LOADOBJ_H__
#define __IO_PUBLIC_LOADOBJ_H__

#include <MeshData.h>
#include <plog/Log.h>
#include <tiny_obj_loader.h>
#include <fstream>
#include <string>

namespace fu {
namespace io {

static MeshData LoadObj(const std::string& filepath)
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
		return MeshData{};
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

	std::vector<float3> vertices;
	std::vector<float2> texcoords;
	std::vector<float3> normals;
	std::vector<uchar4> colors;
	std::vector<uint3>	tindex;
	int numFaces = 0;
	int numVertices = 0;
	float3 bboxMin{ 1.0e16, 1.0e16 , 1.0e16 };
	float3 bboxMax{ -1.0e16, -1.0e16 , -1.0e16 };
	for (auto& it = shapes.begin(); it != shapes.end(); it++)
	{
		const tinyobj::shape_t& shape = *it;
		/// get the polygon type of the shape
		for (uint64_t i = 0; i < shape.mesh.indices.size(); i++)
		{
			const int vIdx = shape.mesh.indices[i].vertex_index;
			const float x = attrib.vertices[3 * vIdx + 0];
			const float y = attrib.vertices[3 * vIdx + 1];
			const float z = attrib.vertices[3 * vIdx + 2];

			float3 vertex = make_float3(x, y, z);
			bboxMax.x = std::max(bboxMax.x, vertex.x);
			bboxMax.y = std::max(bboxMax.y, vertex.y);
			bboxMax.z = std::max(bboxMax.z, vertex.z);
			bboxMin.x = std::min(bboxMin.x, vertex.x);
			bboxMin.y = std::min(bboxMin.y, vertex.y);
			bboxMin.z = std::min(bboxMin.z, vertex.z);

		}
	}

	/*///
	if (shapes.size() > 1)
	{
		LOG_ERROR << "Currently single shape loading is supported";
		return MeshData{};
	}
	MeshData data;
	if (!attrib.vertices.empty())
	{
		int count = attrib.vertices.size() / 3;
		int bsize = attrib.vertices.size() * sizeof(float);
		data.VertexBuffer = CreateBufferCPU<float3>(count);
		std::memcpy(data.VertexBuffer->Data(), attrib.vertices.data(), bsize);
	}
	if (!shapes[0].mesh.indices.empty())
	{
		data.HasFaces = true;
		int count = shapes[0].mesh.indices.size() / 3;
		int bsize = shapes[0].mesh.indices.size() * sizeof(unsigned int);
		data.TIndexBuffer = CreateBufferCPU<uint3>(count);
		std::memcpy(data.TIndexBuffer->Data(), shapes[0].mesh.indices.data(), bsize);
	}
	if (!attrib.normals.empty())
	{
		data.HasNormals = true;
		int count = attrib.normals.size() / 3;
		int bsize = attrib.normals.size() * sizeof(float);
		data.NormalBuffer = CreateBufferCPU<float3>(count);
		std::memcpy(data.NormalBuffer->Data(), attrib.normals.data(), bsize);
	}
	if (!attrib.colors.empty())
	{
		data.HasColors = true;
		int count = attrib.colors.size() / 3;
		int bsize = attrib.colors.size() * sizeof(float);
		/// convert colors to bytes
		data.ColorBuffer = CreateBufferCPU<uchar4>(count);
		std::memcpy(data.ColorBuffer->Data(), attrib.colors.data(), bsize);
	}
	if (!attrib.texcoords.empty())
	{
		data.HasTexcoords = true;
		int count = attrib.texcoords.size() / 2;
		int bsize = attrib.texcoords.size() * sizeof(float);
		data.TexcoordBuffer = CreateBufferCPU<float2>(count);
		std::memcpy(data.TexcoordBuffer->Data(), attrib.texcoords.data(), bsize);
	}
	return data;*/
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_LOADOBJ_H__