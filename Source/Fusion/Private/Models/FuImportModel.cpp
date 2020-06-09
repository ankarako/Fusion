#include <Models/FuImportModel.h>
#include <Core/SettingsRepo.h>
#include <LoadObj.h>
#include <LoadPly.h>
#include <plog/Log.h>
#include <filesystem>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief FuImportModel implementation
struct FuImportModel::Impl
{
	/// \typedef app_ptr_t
	///	\brief a shared pointer to an external app object
	//using app_ptr_t = std::shared_ptr<ExternalApp>;
	/// consts
	static constexpr const char* k_7ZipExePath				= "Resources\\7zip\\7z.exe";
	static constexpr const char* k_TempFolderPath			= "Temp";
	static constexpr const char* k_SkinningFilename			= "skinning.json";
	static constexpr const char* k_SkeletonFilename			= "skeleton.json";
	static constexpr const char* k_TemplateMeshFilenameObj	= "template_mesh.obj";
	static constexpr const char* k_TemplateMeshFilenamePly	= "template_mesh.ply";
	static constexpr const char* k_TextureAtlasFilename		= "texture_atlas.jpg";
	static constexpr const char* k_TrackedParamsFilename	= "tracked_params.json";
	static constexpr const char* k_TexelsFilename			= "texels.avi";

	prj_settings_ptr_t	m_ProjectSettings;
	settings_ptr_t		m_Settings;
	srepo_ptr_t			m_Srepo;

	rxcpp::subjects::subject<std::string>					m_FuFilepathFlowInSubj;
	rxcpp::subjects::subject<prj_settings_ptr_t>			m_PrjSettingsFlowIntSubj;
	rxcpp::subjects::subject<io::MeshData>					m_TemplateMeshFlowOutSubj;
	rxcpp::subjects::subject<io::perfcap_skeleton_ptr_t>	m_SkeletonFlowOutSubj;
	rxcpp::subjects::subject<io::perfcap_skin_data_ptr_t>	m_SkinDataFlowOutSubj;
	rxcpp::subjects::subject<io::tracked_seq_ptr_t>			m_TrackedParamsFlowOutSubj;
	rxcpp::subjects::subject<perfcap_tex_mesh_t>			m_PerfcapMeshFlowOutSubj;

	Impl(srepo_ptr_t srepo) 
		: m_Settings(std::make_shared<FuImportSettings>())
		, m_Srepo(srepo)
	{ }
};	///	!struct Impl
FuImportModel::FuImportModel(srepo_ptr_t srepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(srepo))
{ }

void FuImportModel::Init()
{
	m_Impl->m_Srepo->RegisterSettings(m_Impl->m_Settings);

	m_Impl->m_PrjSettingsFlowIntSubj.get_observable().as_dynamic()
		.subscribe([this](const prj_settings_ptr_t& settings) 
	{
		m_Impl->m_ProjectSettings = settings;
	});

	m_Impl->m_FuFilepathFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::string& filepath) 
	{
		#if _MSC_VER >= 1924
			using namespace std;
		#else		
			using namespace std::experimental;
		#endif
		LOG_INFO << "Attempting to open file: " << filepath;
		/// unzip the the files in project's workspace folder
		std::string fname = filesystem::path(filepath).filename().replace_extension("").generic_string();
		std::string unzipPath = m_Impl->m_ProjectSettings->WorkspacePath + "\\" + fname;
		if (!filesystem::exists(unzipPath))
		{
			filesystem::create_directory(unzipPath);
		}
		std::string cli = m_Impl->k_7ZipExePath + std::string(" e ") + filepath + " -o" + unzipPath;
		LOG_INFO << "Running: " << cli;
		int ret = std::system(cli.c_str());
		if (ret == 0)
		{
			/// load skin data
			std::string skinFPath = unzipPath + "\\" + m_Impl->k_SkinningFilename;
			m_Impl->m_Settings->SkinDataFilepath = skinFPath;
			io::perfcap_skin_data_ptr_t skinData = io::ImportPerfcapSkinningData(skinFPath);
			/// load skeleton
			std::string skeletonFPath = unzipPath + "\\" + m_Impl->k_SkeletonFilename;
			m_Impl->m_Settings->SkeletonFilepath = skeletonFPath;
			io::perfcap_skeleton_ptr_t skeleton = io::ImportPerfcapSkeleton(skeletonFPath);
			/// import mesh
			std::string meshFPath = unzipPath + "\\" + m_Impl->k_TemplateMeshFilenameObj;
			m_Impl->m_Settings->TemplateMeshFilepath = meshFPath;
			io::MeshData meshData = io::CreateMeshData();
			io::perfcap_skin_data_ptr_t denseSkinData = io::CreatePerfcapSkinData();
			io::LoadObjWithSkinData(meshFPath, meshData, skinData, denseSkinData);
			/// load texture video
			std::string trackedFPath = unzipPath + "\\" + m_Impl->k_TrackedParamsFilename;
			m_Impl->m_Settings->TrackedParamsFilepath = trackedFPath;
			io::tracked_seq_ptr_t trackedData = io::ImportPerfcapTrackedParams(trackedFPath);

			std::string texFPath = unzipPath + "\\" + m_Impl->k_TexelsFilename;
			m_Impl->m_Settings->TextureVideoFilepath = texFPath;

			std::string name = filesystem::path(filepath).filename().replace_extension("").generic_string();
			m_Impl->m_Settings->Name = name;
			perfcap_tex_mesh_t perfMesh = std::make_tuple(meshData, skeleton, denseSkinData, trackedData, texFPath, name);
			m_Impl->m_PerfcapMeshFlowOutSubj.get_subscriber().on_next(perfMesh);
		}
	});

	m_Impl->m_Settings->OnSettingsLoaded()
		.subscribe([this](auto _) 
	{
		/// load skin data
		std::string skinFPath = m_Impl->m_Settings->SkinDataFilepath;
		std::string skeletonFPath = m_Impl->m_Settings->SkeletonFilepath;;
		std::string meshFPath = m_Impl->m_Settings->TemplateMeshFilepath;
		std::string trackedFPath = m_Impl->m_Settings->TrackedParamsFilepath;
		std::string texFPath = m_Impl->m_Settings->TextureVideoFilepath;
		std::string name = m_Impl->m_Settings->Name;

		if (!skinFPath.empty() && !skeletonFPath.empty() && !meshFPath.empty() && !trackedFPath.empty() && !texFPath.empty() && !name.empty())
		{
			io::perfcap_skin_data_ptr_t skinData = io::ImportPerfcapSkinningData(skinFPath);
			/// load skeleton
			io::perfcap_skeleton_ptr_t skeleton = io::ImportPerfcapSkeleton(skeletonFPath);
			/// import mesh

			io::MeshData meshData = io::CreateMeshData();
			io::perfcap_skin_data_ptr_t denseSkinData = io::CreatePerfcapSkinData();
			io::LoadObjWithSkinData(meshFPath, meshData, skinData, denseSkinData);
			/// load texture video

			io::tracked_seq_ptr_t trackedData = io::ImportPerfcapTrackedParams(trackedFPath);
			perfcap_tex_mesh_t perfMesh = std::make_tuple(meshData, skeleton, denseSkinData, trackedData, texFPath, name);
			m_Impl->m_PerfcapMeshFlowOutSubj.get_subscriber().on_next(perfMesh);
		}
	});
}

rxcpp::observer<std::string> FuImportModel::FuFilepathFlowIn()
{
	return m_Impl->m_FuFilepathFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<FuImportModel::prj_settings_ptr_t> FuImportModel::PrjSettingsFlowIn()
{
	return m_Impl->m_PrjSettingsFlowIntSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<FuImportModel::perfcap_tex_mesh_t> FuImportModel::PerfcapMeshDataFlowOut()
{
	return m_Impl->m_PerfcapMeshFlowOutSubj.get_observable().as_dynamic();
}


}	///	!namespace fusion
}	///	!namespace fu