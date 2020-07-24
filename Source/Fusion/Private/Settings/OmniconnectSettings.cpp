#include <Settings/OmniconnectSettings.h>

namespace fu {
namespace fusion {
void OmniconnectSettings::Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer)
{
	writer.Key("OmniconnectSettings");
	writer.StartObject();
	{
		writer.Key("VideoListFilepath");
		writer.String(this->VideoListResponseFilepath.c_str());
		writer.Key("SelectedIdx");
		writer.Int(this->SelectedVideoListIndex);
	}
	writer.EndObject();
	this->m_OnSettingsSavedSubj.get_subscriber().on_next(nullptr);
}

void OmniconnectSettings::Load(rapidjson::Document& doc)
{
	auto obj = doc.GetObject();
	if (obj.HasMember("OmniconnectSettings"))
	{
		auto member = obj["OmniconnectSettings"].GetObject();
		if (member.HasMember("VideoListFilepath"))
		{
			this->VideoListResponseFilepath = member["VideoListFilepath"].GetString();
		}
		if (member.HasMember("SelectedIdx"))
		{
			this->SelectedVideoListIndex = member["SelectedIdx"].GetInt();
		}
		this->m_OnSettingsLoadedSubj.get_subscriber().on_next(nullptr);
	}
}
rxcpp::observable<void*> OmniconnectSettings::OnSettingsSaved()
{
	return this->m_OnSettingsSavedSubj.get_observable().as_dynamic();
}
rxcpp::observable<void*> OmniconnectSettings::OnSettingsLoaded()
{
	return this->m_OnSettingsLoadedSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu