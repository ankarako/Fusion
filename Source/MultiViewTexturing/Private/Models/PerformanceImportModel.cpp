#include <Models/PerformanceImportModel.h>
#include <Core/ExternalApp.h>
#include <PerfcapCalibrationImport.h>
#include <DebugMsg.h>
#include <plog/Log.h>
#include <filesystem>

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
	static constexpr const char* k_TemplateMeshFilename		= "template_mesh.ply";
	static constexpr const char* k_TrackedParamsFilename	= "tracked_params.json";
	/// members
	app_ptr_t m_7ZipApp;
	/// inputs
	rxcpp::subjects::subject<std::string> m_ImportFilepathFlowInSubj;
	/// Construction
	Impl() 
		: m_7ZipApp(std::make_shared<ExternalApp>())
	{ 
		m_7ZipApp->ExePath = k_7ZipExePath;
	}
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
		/// first delete the folder resulting unzipped folder
		/// the folder name is the same as the filename
		std::string strippedFname = filesystem::path(filepath).filename().replace_extension("").generic_string();
		DebugMsg(strippedFname);
		std::string pathToDelete = m_Impl->k_TempFolderPath + std::string("\\") + strippedFname;
		if (filesystem::exists(pathToDelete))
		{
			filesystem::remove(pathToDelete);
		}
		/// so lets open the files one by one
		///========================
		/// import calibration file
		///========================
		std::string calibFilepath = m_Impl->k_TempFolderPath + std::string("\\") + m_Impl->k_CalibrationFilename;
		std::vector<io::volcap_cam_data_ptr_t> calib = io::ImportPerfcapCalibration(calibFilepath);
		DebugMsg("Loaded calibration file.");
		///===============
		/// Skinning file
		///===============

		/// delete temporary files
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
}	///	!namespace mvt
}	///	!namespace fu