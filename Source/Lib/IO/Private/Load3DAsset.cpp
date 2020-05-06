#include <Load3DAsset.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <plog/Log.h>

namespace fu {
namespace io {
MeshData Load3DAsset(const std::string& filepath)
{
	Assimp::Importer importer;
	auto importFlags = 
		aiProcess_CalcTangentSpace 		|
		aiProcess_Triangulate 			|
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType			|
		aiProcess_PopulateArmatureData;
	const aiScene* scene = importer.ReadFile(filepath.c_str(), importFlags);

	if (!scene)
		return MeshData{};	
	/// convert scene to our own shit
	/// Check for what data the files has
	int numMeshes = scene->mNumMeshes;
	LOG_DEBUG << "Impoted file: " << filepath;
	LOG_DEBUG << "\tNum Meshes: " << numMeshes;
	for (int i = 0; i < numMeshes; i++)
	{
		auto mesh = scene->mMeshes[i];
		LOG_DEBUG << "\tMesh #" << i;
		LOG_DEBUG << "\t\tNum Vertices: " << mesh->mNumVertices;
		LOG_DEBUG << "\t\tNum Faces   : " << mesh->mNumFaces;
		
	}
}
}	///	!namespace io
}	///	!namespace fu