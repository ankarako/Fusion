#include <Systems/AssetLoadingSystem.h>
#include <ReadFileBytes.h>
//#include <Buffer.h>
#include <filesystem>
#include <map>
#include <vector>
#include <memory>
#include <fstream>
#include <istream>
#include <tinyply.h>
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
	{ "obj" ,		FileType::Obj },
	{ "ply" ,		FileType::Ply },
	{ "perfcap",	FileType::Perfcap }
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
	rxcpp::subjects::subject<TriangleMeshComp>	m_TrangleMeshCompFlowOutSubj;
	rxcpp::subjects::subject<PointCloudComp>		m_PointCloudCompFlowOutSubj;
};	///	!struct Impl
/// \brief load an obj file
///	\param	filepath the path to the file to load
void AssetLoadingSystem::Impl::LoadObj(const std::string& filepath, ContextComp& ctxComp)
{

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
			LOG_DEBUG << std::endl;
		}
	}

	std::shared_ptr<PlyData> vertices, normals, colors, texcoords, faces, tripstrip;

	try { vertices = file.request_properties_from_element("vertex", { "x", "y", "z" }); }
	catch (const std::exception & e) { LOG_ERROR << "tinyply exception: " << e.what() << std::endl; }

	try { normals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
	catch (const std::exception & e) { LOG_ERROR << "tinyply exception: " << e.what() << std::endl; }

	try { colors = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }); }
	catch (const std::exception & e) { LOG_ERROR << "tinyply exception: " << e.what() << std::endl; }

	try { colors = file.request_properties_from_element("vertex", { "r", "g", "b", "a" }); }
	catch (const std::exception & e) { LOG_ERROR << "tinyply exception: " << e.what() << std::endl; }

	try { texcoords = file.request_properties_from_element("vertex", { "u", "v" }); }
	catch (const std::exception & e) { LOG_ERROR << "tinyply exception: " << e.what() << std::endl; }

	// Providing a list size hint (the last argument) is a 2x performance improvement. If you have 
	// arbitrary ply files, it is best to leave this 0. 
	try { faces = file.request_properties_from_element("face", { "vertex_indices" }, 3); }
	catch (const std::exception & e) { LOG_ERROR << "tinyply exception: " << e.what() << std::endl; }

	// Tristrips must always be read with a 0 list size hint (unless you know exactly how many elements
	// are specifically in the file, which is unlikely); 
	try { tripstrip = file.request_properties_from_element("tristrips", { "vertex_indices" }, 0); }
	catch (const std::exception & e) { LOG_ERROR << "tinyply exception: " << e.what() << std::endl; }

	file.read(*fStream);

	if (vertices)	LOG_DEBUG << "\tRead " << vertices->count << "total vertices";
	if (normals)	LOG_DEBUG << "\tRead " << vertices->count << "total normals";
	if (colors)		LOG_DEBUG << "\tRead " << vertices->count << "total colors";
	if (texcoords)	LOG_DEBUG << "\tRead " << vertices->count << "total texcoords";
	if (faces)		LOG_DEBUG << "\tRead " << vertices->count << "total faces";
	if (tripstrip)	LOG_DEBUG << "\tRead " << vertices->count << "total tripstrip";


	if (vertices && faces && !tripstrip)
	{
		/// surely it is a trangle mesh
		/// create a Triangle mesh component
		TriangleMeshComp trMeshComp = CreateTriangleMeshComponent();
		/// get vertex count etc

	}
	else if (vertices && normals && !faces)
	{
		/// we have a point cloud

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
	return m_Impl->m_TrangleMeshCompFlowOutSubj.get_observable().as_dynamic();
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