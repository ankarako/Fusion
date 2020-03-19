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
	LOG_DEBUG << "Number of texcoords: " << (int)(attrib.texcoords.size() / 2);
	LOG_DEBUG << "Number of materials: " << (int)(materials.size());
	LOG_DEBUG << "Number of shapes   : " << (int)(shapes.size());
	
	///
	if (shapes.size() > 1)
	{
		LOG_ERROR << "Currently single shape loading is supported";
		return MeshData{};
	}
	if (!attrib.vertices.empty())
	{
		int bsize = attrib.vertices.size() * sizeof(float);
		
	}
	std::vector<unsigned int> face_indices;
	for (int f = 0; f < shapes[0].mesh.indices.size(); f++)
	{
		face_indices.emplace_back(shapes[0].mesh.indices[f].vertex_index);
	}
	if (!face_indices.empty())
	{
		int bsize = face_indices.size() * sizeof(unsigned int);
		
	}
	bool hasnormals = false;
	
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_LOADOBJ_H__