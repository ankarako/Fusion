#include <Models/PerformanceImportModel.h>
#include <Core/ExternalApp.h>
#include <filesystem>
#include <plog/Log.h>
#include <rapidjson/document.h>
#include <fstream>

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