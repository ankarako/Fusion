#include <Settings/RayTracingSettings.h>

namespace fu {
namespace fusion {

void RayTracingSettings::Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer)
{
	writer.Key("RayTracingSettings");
	writer.StartObject();
	writer.Key("LoadedFiles");
	writer.StartArray();
	for (int i = 0; i < LoadedFiles.size(); i++)
	{
		writer.String(LoadedFiles[i].c_str());
	}
	writer.EndArray();
	writer.EndObject();
	this->m_OnSettingsSavedSubj.get_subscriber().on_next(nullptr);
}

void RayTracingSettings::Load(rapidjson::Document& doc)
{
	auto obj = doc.GetObject();
	if (obj.HasMember("RayTracingSettings"))
	{
		auto member = obj["RayTracingSettings"].GetObject();
		if (member.HasMember("LoadedFiles"))
		{
			if (!LoadedFiles.empty())
				LoadedFiles.clear();
			auto arr = member["LoadedFiles"].GetArray();
			for (int i = 0; i < arr.Size(); i++)
			{
				LoadedFiles.emplace_back(arr[i].GetString());
			}
		}
	}
	this->m_OnSettingsLoadedSubj.get_subscriber().on_next(nullptr);
}

rxcpp::observable<void*> RayTracingSettings::OnSettingsSaved()
{
	return m_OnSettingsSavedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> RayTracingSettings::OnSettingsLoaded()
{
	return m_OnSettingsLoadedSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu