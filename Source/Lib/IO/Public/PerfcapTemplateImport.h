#ifndef __IO_PUBLIC_PERFCAPTEMPLATEIMPORT_H__
#define __IO_PUBLIC_PERFCAPTEMPLATEIMPORT_H__

#include <MeshData.h>
#include <tinyply.h>
#include <tiny_obj_loader.h>
#include <string>
#include <fstream>
#include <plog/Log.h>

namespace fu {
namespace io {
	///
static void PerfcapMeshImport(const std::string& plyFilepath, const std::string objFilepath, io::MeshData& data)
{
	using namespace tinyply;
	std::unique_ptr<std::istream> fStream;
	using byte_t = unsigned char;
	std::vector<byte_t> byteBuffer;
	fStream.reset(new std::ifstream(plyFilepath, std::ios::binary));

	if (!fStream || fStream->fail())
		throw std::runtime_error("Failed to read file stream. File: " + plyFilepath);

	fStream->seekg(0, std::ios::end);
	const float szmb = fStream->tellg() * float(1e-6);
	fStream->seekg(0, std::ios::beg);

	PlyFile file;
	file.parse_header(*fStream);

	LOG_DEBUG << "\t[ply_header] Type: " << (file.is_binary_file() ? "binary" : "ascii");
	for (auto& c : file.get_comments())	LOG_DEBUG << "\t[ply_header] Comment: " << c;
	for (auto& c : file.get_info())		LOG_DEBUG << "\t[ply_header] Info   : " << c;

	std::shared_ptr<PlyData> vertices, normals;

	try { vertices = file.request_properties_from_element("vertex", { "x", "y", "z" }); }
	catch (const std::exception & e) { LOG_WARNING << "tinyply exception: " << e.what(); }

	try { normals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
	catch (const std::exception & e) { LOG_WARNING << "tinyply exception: " << e.what(); }

	file.read(*fStream);

	if (vertices)	LOG_DEBUG << "\tRead " << vertices->count << "total vertices";
	if (normals)	LOG_DEBUG << "\tRead " << vertices->count << "total normals";

	if (vertices)
	{
		int count = vertices->count;
		int bsize = vertices->buffer.size_bytes();
		data->VertexBuffer = CreateBufferCPU<float3>(count);
		std::memcpy(data->VertexBuffer->Data(), vertices->buffer.get(), bsize);
	}
	if (normals)
	{
		data->HasNormals = true;
		int count = normals->count;
		int bsize = normals->buffer.size_bytes();
		data->NormalBuffer = CreateBufferCPU<float3>(count);
		std::memcpy(data->NormalBuffer->Data(), normals->buffer.get(), bsize);
	}

	using namespace tinyobj;
	tinyobj::attrib_t					attrib;
	std::vector<tinyobj::shape_t>		shapes;
	std::vector<tinyobj::material_t>	materials;
	std::string warn;
	std::string err;

	std::ifstream infd(objFilepath);
	if (!infd.good())
	{
		LOG_ERROR << "Failed to open file: " << objFilepath;
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
		LOG_ERROR << "Failed to open file: " << objFilepath;
		return;
	}
	LOG_DEBUG << "[Obj Loader]";
	LOG_DEBUG << "Parsed file: " << objFilepath;
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
	std::vector<float2> texcoords;
	tinyobj::shape_t shape = shapes[0];
	int tid = 0;
	for (int t = 0; t < shape.mesh.indices.size(); ++t)
	//for (const auto& index : shape.mesh.indices)
	{
		auto index = shape.mesh.indices[t];
		float2 texcoord = make_float2(
			attrib.texcoords[2 * index.texcoord_index + 0],
			attrib.texcoords[2 * index.texcoord_index + 1]
		);

		//float3 normal = make_float3(
		//	attrib.normals[3 * index.normal_index + 0],
		//	attrib.normals[3 * index.normal_index + 1],
		//	attrib.normals[3 * index.normal_index + 2]
		//);

		/*uint3 tindex = make_uint3(
			3 * index.vertex_index + 0,
			3 * index.vertex_index + 1,
			3 * index.vertex_index + 2
		);*/
		texcoords.emplace_back(texcoord);
		indices.emplace_back(index.vertex_index);
		tid++;
	}
	if (!indices.empty())
	{
		data->HasFaces = true;
		int count = indices.size() / 3;
		int bsize = indices.size() * sizeof(unsigned int);
		data->TIndexBuffer = CreateBufferCPU<uint3>(count);
		std::memcpy(data->TIndexBuffer->Data(), indices.data(), bsize);
	}
	if (!texcoords.empty())
	{
		data->HasTexcoords = true;
		int count = texcoords.size();
		int bsize = texcoords.size() * sizeof(float2);
		data->TexcoordBuffer = CreateBufferCPU<float2>(count);
		std::memcpy(data->TexcoordBuffer->Data(), texcoords.data(), bsize);
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
}
}
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAPTEMPLATEIMPORT_H__