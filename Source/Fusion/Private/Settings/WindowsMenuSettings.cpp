#include <Settings/WindowsMenuSettings.h>

namespace fu {
namespace fusion {
/// WindowsMenuSettings Implementation
/// \brief save settings to a pretty writer
///	\param	writer	the pretty writer to save our settings into
void WindowsMenuSettings::Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer)
{
	writer.Key("WindowsMenuSettings");
	writer.StartObject();
	writer.Key("NormalsWindowVisible");
	writer.Bool(this->NormalsWindowVisible);
	writer.Key("IlluminationWindowVisible");
	writer.Bool(this->IlluminationWindowVisible);
	writer.Key("ViewportOptionsVisible");
	writer.Bool(this->ViewpotOptionsVisible);
	writer.EndObject();
	m_OnSettingsSavedSubj.get_subscriber().on_next(nullptr);
}
///	\brief load settings from a reapidjson document
///	\param	doc	the document to load our settings from
void WindowsMenuSettings::Load(rapidjson::Document& doc)
{
	auto& obj = doc.GetObject();
	if (obj.HasMember("WindowsMenuSettings"))
	{
		auto& member = obj["WindowsMenuSettings"].GetObject();
		if (member.HasMember("NormalsWindowVisible"))
		{
			this->NormalsWindowVisible = member["NormalsWindowVisible"].GetBool();
		}
		if (member.HasMember("IlluminationWindowVisible"))
		{
			this->IlluminationWindowVisible = member["IlluminationWindowVisible"].GetBool();
		}
		if (member.HasMember("ViewportOptionsVisible"))
		{
			this->ViewpotOptionsVisible = member["ViewportOptionsVisible"].GetBool();
		}
		m_OnSettingsLoadedSubj.get_subscriber().on_next(nullptr);
	}
	
}

rxcpp::observable<void*> WindowsMenuSettings::OnSettingsSaved()
{
	return m_OnSettingsSavedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> WindowsMenuSettings::OnSettingsLoaded()
{
	return m_OnSettingsLoadedSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu