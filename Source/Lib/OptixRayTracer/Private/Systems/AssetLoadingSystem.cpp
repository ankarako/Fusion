#include <Systems/AssetLoadingSystem.h>
#include <Systems/MeshMappingSystem.h>
#include <ReadFileBytes.h>
//#include <Buffer.h>
#include <filesystem>
#include <map>
#include <vector>
#include <memory>
#include <fstream>
#include <istream>
#include <tinyply.h>
#include <tiny_obj_loader.h>
#include <plog/Log.h>

namespace fu {
namespace rt {
struct Map3DComponents
{

};
///	\enum FileType
enum class FileType
{
	Obj,
	Ply,
	Perfcap
};
/// \brief maps strings to filetypes
const std::map<std::string, FileType> String2FileType =
{
	{ ".obj" ,		FileType::Obj },
	{ ".ply" ,		FileType::Ply },
	{ ".perfcap",	FileType::Perfcap }
};
///	\struct Impl
///	\brief AssetLoadingSystem implementation
struct AssetLoadingSystem::Impl
{
	/// \brief load an obj file
	///	\param	filepath the path to the file to load
	void LoadObj(const std::string& filepath, ContextComp& ctxComp);
	///	\brief load ply
	///	\param	filepath	the path to the file to load
	void LoadPly(const std::string& filepth, ContextComp& ctxComp);
	///	\brief load a perfcap file
	///	\param	filepath	the patht to the file to load
	void LoadPerfcap(const std::string& fileapth, ContextComp& ctxComp);
	/// Loads obj files
	rxcpp::subjects::subject<TriangleMeshComp>		m_TriangleMeshCompFlowOutSubj;
	rxcpp::subjects::subject<PointCloudComp>		m_PointCloudCompFlowOutSubj;
};	///	!struct Impl
/// \brief load an obj file
///	\param	filepath the path to the file to load
void AssetLoadingSystem::Impl::LoadObj(const std::string& filepath, ContextComp& ctxComp)
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
		return;
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
		return;
	}
	LOG_DEBUG << "[Obj Loader]";
	LOG_DEBUG << "Parsed file: " << filepath;
	LOG_DEBUG << "[attributes]";
	LOG_DEBUG << "Number of vertices : " << (int)(attrib.vertices.size() / 3);
	LOG_DEBUG << "Number of normals  : " << (int)(attrib.normals.size() / 3);
	LOG_DEBUG << "Number of texcoords: " << (int)(attrib.texcoords.size() / 2);
	LOG_DEBUG << "Number of materials: " << (int)(materials.size());
	LOG_DEBUG << "Number of shapes   : " << (int)(shapes.size());
}
///	\brief load ply
///	\param	filepath	the path to the file to load
void AssetLoadingSystem::Impl::LoadPly(const std::string& filepath, ContextComp& ctxComp)
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

	try { colors = file.request_properties_from_element("vertex", { "red", "green", "blue"}); }
	catch (const std::exception & e) { LOG_WARNING << "tinyply exception: " << e.what(); }

	try { colors = file.request_properties_from_element("vertex", { "r", "g", "b", "a" }); }
	catch (const std::exception & e) { LOG_WARNING << "tinyply exception: " << e.what(); }

	try { colors = file.request_properties_from_element("vertex", { "r", "g", "b"}); }
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
	/// now we have to check if the file is a triangle mesh or a point cloud
	/// FIXME: way to determine point cloud to mesh
	bool isTriangleMesh		= vertices  && faces != nullptr && normals && !colors;
	if (isTriangleMesh)
	{
		/// surely it is a trangle mesh
		/// create a Triangle mesh component
		TriangleMeshComp trMeshComp = CreateTriangleMeshComponent();
		/// get vertex count etc
		if (vertices)
		{
			int num_vertices = vertices->count;
			int vertex_bsize = vertices->buffer.size_bytes();
			MeshMappingSystem::CreateTriangleMeshVertexBuffer(trMeshComp, ctxComp, num_vertices);
			MeshMappingSystem::MapTriangleMeshVertexBuffer(trMeshComp, (void*)vertices->buffer.get(), vertex_bsize);
		}
		if (normals)
		{
			int num_vertices = normals->count;
			int bsize = normals->buffer.size_bytes();
			MeshMappingSystem::CreateTriangleMeshNormalBuffer(trMeshComp, ctxComp, num_vertices);
			MeshMappingSystem::MapTriangleMeshNormalBuffer(trMeshComp, (void*)normals->buffer.get(), bsize);
		}
		if (faces)
		{
			int numFaces = faces->count;
			int bsize = faces->buffer.size_bytes();
			MeshMappingSystem::CreateTriangleMeshFaceBuffer(trMeshComp, ctxComp, numFaces);
			MeshMappingSystem::MapTriangleMeshFaceBuffer(trMeshComp, (void*)faces->buffer.get(), bsize);
		}
		if (texcoords)
		{
			int numTexcoords = texcoords->count;
			int bsize = texcoords->buffer.size_bytes();
			MeshMappingSystem::CreateTriangleMeshTexCoordBuffer(trMeshComp, ctxComp, numTexcoords);
			MeshMappingSystem::MapTriangleMeshTexCoordBuffer(trMeshComp, (void*)texcoords->buffer.get(), bsize);
		}
		/// create and attach hitgroup programs
		/// send mesh to flow out
	}
	else
	{
		/// we have a point cloud
		PointCloudComp pCloufComp = CreatePointCloudComponent();
		/// get vertex count etc
		if (vertices)
		{
			int numVertices = vertices->count;
			int bsize = vertices->buffer.size_bytes();
			MeshMappingSystem::CreatePointCloudVertexBuffer(pCloufComp, ctxComp, numVertices);
			MeshMappingSystem::MapPointCloudVertexBuffer(pCloufComp, (void*)vertices->buffer.get(), bsize);
		}
		if (normals)
		{
			int numNormals = normals->count;
			int bsize = normals->buffer.size_bytes();
			MeshMappingSystem::CreatePointCloudNormalBuffer(pCloufComp, ctxComp, numNormals);
			MeshMappingSystem::MapPointCloudNormalBuffer(pCloufComp, (void*)normals->buffer.get(), bsize);
		}
		if (colors)
		{
			int numColors = colors->count;
			int bsize = colors->buffer.size_bytes();
			MeshMappingSystem::CreatePointCloudColorBuffer(pCloufComp, ctxComp, numColors);
			MeshMappingSystem::MapPointCloudColorBuffer(pCloufComp, (void*)colors->buffer.get(), bsize);
		}
		MeshMappingSystem::MapPointCloudComponent(pCloufComp, ctxComp, vertices->count);
		///	send to point cloud flow out
		m_PointCloudCompFlowOutSubj.get_subscriber().on_next(pCloufComp);
	}
}
///	\brief load a perfcap file
///	\param	filepath	the patht to the file to load
void AssetLoadingSystem::Impl::LoadPerfcap(const std::string& fileapth, ContextComp& ctxComp)
{

}
/// Construction
AssetLoadingSystem::AssetLoadingSystem()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
/// outputs
/// \brief	Triangle mesh output
///	if the system is called to load an asset, and determines that
///	it is a triangle mesh, it will come out frome here
rxcpp::observable<TriangleMeshComp> AssetLoadingSystem::TriangleMeshFlowOut()
{
	return m_Impl->m_TriangleMeshCompFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<PointCloudComp> fu::rt::AssetLoadingSystem::PointCloudFlowOut()
{
	return m_Impl->m_PointCloudCompFlowOutSubj.get_observable().as_dynamic();
}
///	\brief load an asset
///	top level loading function. It determines the asset type and
///	\param	filepath	the path to the file to load
void AssetLoadingSystem::LoadAsset(const std::string& filepath, ContextComp& ctxComp)
{
	using namespace std::filesystem;
	/// convert string to path
	std::filesystem::path filep = path(filepath);
	/// get extension
	std::string ext = filep.extension().generic_string();
	FileType type;
	auto it = String2FileType.find(ext);
	/// We do not support this file type
	if (it == String2FileType.end())
		return;

	type = String2FileType.at(ext);
	switch (type)
	{
	case fu::rt::FileType::Obj:
		m_Impl->LoadObj(filepath, ctxComp);
		break;
	case fu::rt::FileType::Ply:
		m_Impl->LoadPly(filepath, ctxComp);
		break;
	case fu::rt::FileType::Perfcap:
		break;
	default:
		break;
	}
}
}	///	!namespace rt
}	///	!namespace fu