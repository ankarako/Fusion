#ifndef __IO_PUBLIC_SAVETEXTURINGPLY_H__
#define __IO_PUBLIC_SAVETEXTURINGPLY_H__

#include <MeshData.h>
#include <tinyply.h>
#include <string>
#include <fstream>

namespace fu {
namespace io {
///	\brief save a mesh for texture stiching
static void SavePly(const std::string& filepath, const io::MeshData& data)
{
	using namespace tinyply;

	std::filebuf fb_ascii;
	fb_ascii.open(filepath, std::ios::out);
	std::ostream outstream_ascii(&fb_ascii);
	if (outstream_ascii.fail())
		throw std::runtime_error("failed to open filestream: " + filepath);

	int vertexCount = data->VertexBuffer->Count();
	int normalCount = data->NormalBuffer->Count();
	int texcoordCount = data->TexcoordBuffer->Count();
	int faceCount = data->TIndexBuffer->Count();
	
	PlyFile mesh_file;
	mesh_file.add_properties_to_element(
		"vertex", 
		{ "x", "y", "z" }, 
		Type::FLOAT32,
		vertexCount,
		reinterpret_cast<uint8_t*>(data->VertexBuffer->Data()), 
		Type::INVALID, 0);

	mesh_file.add_properties_to_element(
		"face",
		{ "vertex_indices" },
		Type::UINT32,
		faceCount,
		reinterpret_cast<uint8_t*>(data->TIndexBuffer->Data()),
		Type::UINT8, 3);

	mesh_file.add_properties_to_element(
		"face",
		{ "texcoord" },
		Type::FLOAT32,
		vertexCount,
		reinterpret_cast<uint8_t*>(data->TexcoordBuffer->Data()),
		Type::UINT8, 6
	);
	mesh_file.write(outstream_ascii, false);
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_SAVETEXTURINGPLY_H__