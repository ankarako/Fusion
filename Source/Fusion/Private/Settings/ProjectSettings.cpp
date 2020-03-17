#include <Settings/ProjectSettings.h>
#include <fstream>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>

namespace fu {
namespace fusion {

void ProjectSettings::Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer)
{
	writer.Key("ProjectSettings");
	writer.StartObject();
	writer.Key("ProjectPath");
	writer.String(ProjectPath.c_str());
	writer.Key("WorkspacePath");
	writer.String(WorkspacePath.c_str());
	writer.Key("ProjectName");
	writer.String(ProjectName.c_str());
	writer.EndObject();
	m_OnSettingsSavedSubj.get_subscriber().on_next(nullptr);
}

void fu::fusion::ProjectSettings::Load(rapidjson::Document& doc)
{
	auto obj = doc.GetObject();
	if (obj.HasMember("ProjectSettings"))
	{
		auto member = obj["ProjectSettings"].GetObject();
		if (member.HasMember("ProjectPath"))
		{
			this->ProjectPath = member["ProjectPath"].GetString();
		}
		if (member.HasMember("WorkspacePath"))
		{
			this->WorkspacePath = member["WorkspacePath"].GetString();
		}
		if (member.HasMember("ProjectName"))
		{
			this->ProjectName = member["ProjectName"].GetString();
		}
	}
	this->m_OnSettingsLoadedSubj.get_subscriber().on_next(nullptr);
}

rxcpp::observable<void*> fu::fusion::ProjectSettings::OnSettingsLoaded()
{
	return m_OnSettingsLoadedSubj.get_observable().as_dynamic();
}
rxcpp::observable<void*> fu::fusion::ProjectSettings::OnSettingsSaved()
{
	return m_OnSettingsSavedSubj.get_observable().as_dynamic();
}

}	///	!namesapce fusion
}	///	!namespace fu