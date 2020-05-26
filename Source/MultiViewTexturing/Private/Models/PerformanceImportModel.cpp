#include <Models/PerformanceImportModel.h>
#include <Core/ExternalApp.h>
#include <PerfcapCalibrationImport.h>
#include <PerfcapSkeletonImport.h>
#include <PerfcapSkinningDataImport.h>
#include <PerfcapTrackedParamsImport.h>
#include <filesystem>
#include <plog/Log.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief PerformanceImportModel Implementation
struct PerformanceImportModel::Impl
{
	/// \typedef app_ptr_t
	///	\brief a shared pointer to an external app object
	using app_ptr_t = std::shared_ptr<ExternalApp>;
	/// consts
	static constexpr const char* k_7ZipExePath				= "Resources\\7zip\\7z.exe";
	static constexpr const char* k_TempFolderPath			= "Temp";
	static constexpr const char* k_CalibrationFilename		= "calibration.json";
	static constexpr const char* k_SkinningFilename			= "skinning.json";
	static constexpr const char* k_SkeletonFilename			= "skeleton.json";
	static constexpr const char* k_TemplateMeshFilename		= "template_mesh.obj";
	static constexpr const char* k_TextureAtlasFilename		= "texture_atlas.jpg";
	static constexpr const char* k_TrackedParamsFilename	= "tracked_params.json";
	/// members
	app_ptr_t m_7ZipApp;
	/// inputs
	rxcpp::subjects::subject<std::string> m_ImportFilepathFlowInSubj;
	/// outputs
	rxcpp::subjects::subject<std::string>								m_TemplateMeshFilepathFlowOutSubj;
	rxcpp::subjects::subject<std::vector<std::string>>					m_TextureFilenamesFlowOutSubj;
	rxcpp::subjects::subject<std::string>								m_TextureAtlasFilenameFlowOutSubj;
	rxcpp::subjects::subject<io::perfcap_skeleton_ptr_t>				m_PerfcapSkeletonFlowOutSubj;
	rxcpp::subjects::subject<io::perfcap_skin_data_ptr_t>				m_PerfcapSkinDataFlowOutSubj;
	rxcpp::subjects::subject<std::vector<io::volcap_cam_data_ptr_t>> 	m_ViewportDataFlowOutSubj;
	rxcpp::subjects::subject<io::tracked_seq_ptr_t>						m_TrackedSequenceFlowOutSubj;
	/// Construction
	Impl() 
		: m_7ZipApp(std::make_shared<ExternalApp>())
	{ 
		m_7ZipApp->ExePath = k_7ZipExePath;
	}
	/// \brief import calibration json file
	///	\param filename
	/// \brief import skelelton file
	///	\param	filepath
	///	\return 
};	///	!struct Impl
/// Construction
PerformanceImportModel::PerformanceImportModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
/// \brief initialize the model
void PerformanceImportModel::Init()
{
	///=========================
	/// Filepath for importing
	///=========================
	m_Impl->m_ImportFilepathFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::string& filepath)
	{
		using namespace std::experimental;

		LOG_INFO << "Importing performance file: " << filepath;
		LOG_INFO << "Unwrapping file...";
		/// unzip file to temporay folder
		std::string targetDir = m_Impl->k_TempFolderPath;
		if (!filesystem::exists(targetDir))
		{
			filesystem::create_directory(targetDir);
		}
		m_Impl->m_7ZipApp->Cli = "e " + filepath + " -o" + targetDir;
		m_Impl->m_7ZipApp->Run();
		/// so lets open the files one by one
		/// import calibration file
		std::string calibrationFilepath = targetDir + "\\" + m_Impl->k_CalibrationFilename;
		std::vector<io::volcap_cam_data_ptr_t> deviceData = io::ImportPerfcapCalibration(calibrationFilepath);
		m_Impl->m_ViewportDataFlowOutSubj.get_subscriber().on_next(deviceData);
		/// import skeleton
		std::string skeletonFilepath = targetDir + "\\" + m_Impl->k_SkeletonFilename;
		io::perfcap_skeleton_ptr_t skeleton = io::ImportPerfcapSkeleton(skeletonFilepath);
		m_Impl->m_PerfcapSkeletonFlowOutSubj.get_subscriber().on_next(skeleton);
		/// import skinning data
		std::string skinningDataFilepath = targetDir + "\\" + m_Impl->k_SkinningFilename;
		io::perfcap_skin_data_ptr_t skinData = io::ImportPerfcapSkinningData(skinningDataFilepath);
		m_Impl->m_PerfcapSkinDataFlowOutSubj.get_subscriber().on_next(skinData);
		/// find video filenames
		std::vector<std::string> videoFilepaths;
		for (auto path : filesystem::directory_iterator(targetDir))
		{
			std::string ext = path.path().extension().generic_string();
			if (ext == ".avi")
			{
				videoFilepaths.emplace_back(filesystem::absolute(path.path()).generic_string());
			}
		}
		m_Impl->m_TextureFilenamesFlowOutSubj.get_subscriber().on_next(videoFilepaths);
		/// template mesh
		std::string templateMeshFilepath = filesystem::absolute(targetDir + "\\" + m_Impl->k_TemplateMeshFilename).generic_string();
		m_Impl->m_TemplateMeshFilepathFlowOutSubj.get_subscriber().on_next(templateMeshFilepath);
		/// texture atlas
		std::string textureAtlasFilepath = filesystem::absolute(targetDir + "\\" + m_Impl->k_TextureAtlasFilename).generic_string();
		m_Impl->m_TextureAtlasFilenameFlowOutSubj.get_subscriber().on_next(textureAtlasFilepath);
		/// import tracked params
		std::string trackedParamsFilepath = targetDir + "\\" + m_Impl->k_TrackedParamsFilename;
		io::tracked_seq_ptr_t trackedSeq = io::ImportPerfcapTrackedParams(trackedParamsFilepath);
		m_Impl->m_TrackedSequenceFlowOutSubj.get_subscriber().on_next(trackedSeq);
	});
}
/// \brief destroy the model
///	Deletes temporary saved data
void PerformanceImportModel::Destroy()
{
	using namespace std::experimental;
	
	if (filesystem::exists(m_Impl->k_TempFolderPath))
	{
		for (auto path : filesystem::directory_iterator(m_Impl->k_TempFolderPath))
		{
			if (!filesystem::remove(path))
			{
				LOG_ERROR << "Failed to delete: " << path;
			}
		}
	}
}

rxcpp::observer<std::string> fu::mvt::PerformanceImportModel::ImportFilepathFlowIn()
{
	return m_Impl->m_ImportFilepathFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observable<std::string> PerformanceImportModel::TemplateMeshFilepathFlowOut()
{
	return m_Impl->m_TemplateMeshFilepathFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<std::vector<std::string>> PerformanceImportModel::VideoTextureFilepathsFlowOut()
{
	return	m_Impl->m_TextureFilenamesFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<std::string> PerformanceImportModel::TextureAtlasFilepathFlowOut()
{
	return m_Impl->m_TextureAtlasFilenameFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<io::perfcap_skeleton_ptr_t> PerformanceImportModel::SkeletonFlowOut()
{
	return m_Impl->m_PerfcapSkeletonFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<io::perfcap_skin_data_ptr_t> PerformanceImportModel::SkinDataFlowOut()
{
	return m_Impl->m_PerfcapSkinDataFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<std::vector<io::volcap_cam_data_ptr_t>> PerformanceImportModel::ViewportDataFlowOut()
{
	return m_Impl->m_ViewportDataFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<io::tracked_seq_ptr_t> PerformanceImportModel::TrackedSequenceDataFlowOut()
{
	return m_Impl->m_TrackedSequenceFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace mvt
}	///	!namespace fu