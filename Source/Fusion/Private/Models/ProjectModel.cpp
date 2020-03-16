#include <Models/ProjectModel.h>
#include <Settings/ProjectSettings.h>
#include <Core/SettingsRepo.h>
#include <filesystem>
#include <plog/Log.h>

namespace fu {
namespace fusion {
struct ProjectModel::Impl
{
	using settings_ptr_t = std::shared_ptr<ProjectSettings>;

	settings_ptr_t	m_Settings;
	srepo_ptr_t		m_Srepo;
	bool			m_IsSaved{ false };

	rxcpp::subjects::subject<DirEntry> m_CreateNewProjectFlowInSubj;
	rxcpp::subjects::subject<DirEntry> m_LoadProjectFlowInSubj;
	/// Construction
	Impl(srepo_ptr_t srepo)
		: m_Settings(std::make_shared<ProjectSettings>())
		, m_Srepo(srepo)
	{ }
};
/// Construction
ProjectModel::ProjectModel(srepo_ptr_t srepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(srepo))
{ }
///
void ProjectModel::Init()
{
	m_Impl->m_Srepo->RegisterSettings(m_Impl->m_Settings);

	m_Impl->m_CreateNewProjectFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const DirEntry& entry)
	{
		
		if (!m_Impl->m_IsSaved)
		{
			CreateProject(entry.AbsPath);
		}
		else if (m_Impl->m_IsSaved && entry.AbsPath != m_Impl->m_Settings->ProjectPath)
		{
			CreateProject(entry.AbsPath);
		}
		else
		{
			Save();
		}
	});

	m_Impl->m_LoadProjectFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const DirEntry& entry) 
	{
		LoadProject(entry.AbsPath);
	});
}
/// \brief create a new project
///	\param	path
void ProjectModel::CreateProject(const std::string& path)
{
	if (!std::filesystem::exists(path))
	{
		try
		{
			std::string name = std::filesystem::path(path).filename().generic_string();
			std::filesystem::create_directory(path);
			std::filesystem::create_directory(path + "/workspace");
			std::string abspath = path + "/" + name + ".fuproj";
			m_Impl->m_Settings->ProjectName = name;
			m_Impl->m_Settings->ProjectPath = path;
			m_Impl->m_Settings->WorkspacePath = path + "/workspace";
			m_Impl->m_Srepo->Save(abspath);
			m_Impl->m_IsSaved = true;
		}
		catch (std::exception& ex)
		{
			LOG_ERROR << "Failed to create project: " << path << " Error: " << ex.what();
		}
	}
}

void ProjectModel::Save()
{
	std::string abspath = m_Impl->m_Settings->ProjectPath + "/" + m_Impl->m_Settings->ProjectName + ".fuproj";
	m_Impl->m_Srepo->Save(abspath);
}

void ProjectModel::LoadProject(const std::string& filepath)
{
	if (std::filesystem::exists(filepath) && std::filesystem::path(filepath).extension.generic_string() == ".fuproj")
	{
		m_Impl->m_Srepo->Load(filepath);
	}
}

const std::string& ProjectModel::WorkSpaceDirectory() const
{
	return m_Impl->m_Settings->WorkspacePath;
}

bool ProjectModel::IsProjectSaved() const
{
	return m_Impl->m_IsSaved;
}

rxcpp::observer<DirEntry> fu::fusion::ProjectModel::CreateNewProjectFlowIn()
{
	return m_Impl->m_CreateNewProjectFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<DirEntry> fu::fusion::ProjectModel::LoadProjectFlowIn()
{
	return m_Impl->m_LoadProjectFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu