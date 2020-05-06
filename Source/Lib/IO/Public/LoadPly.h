#ifndef __IO_PUBLIC_LOADPLY_H__
#define __IO_PUBLIC_LOADPLY_H__

#include <MeshData.h>
#include <tinyply.h>
#include <plog/Log.h>
#include <string>
#include <fstream>

namespace fu {
namespace io {

static MeshData LoadPly(const std::string& filepath)
{
	using namespace tinyply;
	std::unique_ptr<std::istream> fStream;
	using byte_t = unsigned char;
	std::vector<byte_t> byteBuffer;
	fStream.reset(new std::ifstream(filepath, std::ios::binary));

	if (!fStream || fStream->fail())
		throw std::runtime_error("Failed to read file stream. File: " + filepath);

	fStream->seekg(0, std::ios::end);
	const float szmb = fStream->tellg() * float(1e-6);
	fStream->seekg(0, std::ios::beg);

	PlyFile file;
	file.parse_header(*fStream);

	LOG_DEBUG << "\t[ply_header] Type: " << (file.is_binary_file() ? "binary" : "ascii");
	for (auto& c : file.get_comments())	LOG_DEBUG << "\t[ply_header] Comment: " << c;
	for (auto& c : file.get_info())		LOG_DEBUG << "\t[ply_header] Info   : " << c;

	/// get ply elements
	for (const auto& e : file.get_elements())
	{
		LOG_DEBUG << "\t[ply_header] Element: " << e.name << "(" << e.size << ")";
		for (const auto& p : e.properties)
		{
			LOG_DEBUG << "\t[ply_header] \tproperty: " << p.name << "(type = " << tinyply::PropertyTable[p.propertyType].str << ")";
			if (p.isList)	LOG_DEBUG << "(list_type = " << tinyply::PropertyTable[p.listType].str << ")";
		}
	}

	std::shared_ptr<PlyData> vertices, normals, colors, texcoords, faces, tripstrip;

	try { vertices = file.request_properties_from_element("vertex", { "x", "y", "z" }); }
	catch (const std::exception & e) { LOG_WARNING << "tinyply exception: " << e.what(); }

	try { normals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
	catch (const std::exception & e) { LOG_WARNING << "tinyply exception: " << e.what(); }

	try { colors = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }); }
	catch (const std::exception & e) { LOG_WARNING << "tinyply exception: " << e.what(); }

	try { colors = file.request_properties_from_element("vertex", { "red", "green", "blue" }); }
	catch (const std::exception & e) { LOG_WARNING << "tinyply exception: " << e.what(); }

	try { colors = file.request_properties_from_element("vertex", { "r", "g", "b", "a" }); }
	catch (const std::exception & e) { LOG_WARNING << "tinyply exception: " << e.what(); }

	try { colors = file.request_properties_from_element("vertex", { "r", "g", "b" }); }
	catch (const std::exception & e) { LOG_WARNING << "tinyply exception: " << e.what(); }

	try { texcoords = file.request_properties_from_element("vertex", { "u", "v" }); }
	catch (const std::exception & e) { LOG_WARNING << "tinyply exception: " << e.what(); }

	// Providing a list size hint (the last argument) is a 2x performance improvement. If you have 
	// arbitrary ply files, it is best to leave this 0. 
	try { faces = file.request_properties_from_element("face", { "vertex_indices" }, 3); }
	catch (const std::exception & e) { LOG_WARNING << "tinyply exception: " << e.what(); }

	// Tristrips must always be read with a 0 list size hint (unless you know exactly how many elements
	// are specifically in the file, which is unlikely); 
	try { tripstrip = file.request_properties_from_element("tristrips", { "vertex_indices" }, 0); }
	catch (const std::exception & e) { LOG_WARNING << "tinyply exception: " << e.what(); }

	file.read(*fStream);

	if (vertices)	LOG_DEBUG << "\tRead " << vertices->count << "total vertices";
	if (normals)	LOG_DEBUG << "\tRead " << vertices->count << "total normals";
	if (colors)		LOG_DEBUG << "\tRead " << vertices->count << "total colors";
	if (texcoords)	LOG_DEBUG << "\tRead " << vertices->count << "total texcoords";
	if (faces)		LOG_DEBUG << "\tRead " << vertices->count << "total faces";
	if (tripstrip)	LOG_DEBUG << "\tRead " << vertices->count << "total tripstrip";
	/// now lets copy our data
	MeshData data = CreateMeshData();
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
	if (colors)
	{
		data->HasColors = true;
		int count = colors->count;
		int bsize = colors->buffer.size_bytes();
		data->ColorBuffer = CreateBufferCPU<uchar4>(count);
		std::memcpy(data->ColorBuffer->Data(), colors->buffer.get(), bsize);
	}
	if (faces)
	{
		data->HasFaces = true;
		int count = faces->count;
		int bsize = faces->buffer.size_bytes();
		data->TIndexBuffer = CreateBufferCPU<uint3>(count);
		std::memcpy(data->TIndexBuffer->Data(), faces->buffer.get(), bsize);
	}
	if (texcoords)
	{
		data->HasTexcoords = true;
		int count = texcoords->count;
		int bsize = texcoords->buffer.size_bytes();
		data->TexcoordBuffer = CreateBufferCPU<float2>(count);
		std::memcpy(data->TexcoordBuffer->Data(), texcoords->buffer.get(), bsize);
	}
	return data;
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_LOADPLY_H__