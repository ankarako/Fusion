#include <Settings/PlayerSettings.h>
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
/// PlayerSettings Implementation
///	\brief save settings to a pretty writer
///	\param	writer	the pretty writer to save our settings
void PlayerSettings::Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer)
{
	writer.Key("PlayerSettings");
	writer.StartObject();
	writer.Key("LoadedFilepath");
	writer.String(this->LoadedVideoFilepath.c_str());
	writer.Key("CurrentFrameId");
	writer.Uint64(this->CurrentFrameId);
	writer.EndObject();
	this->m_OnSettingsSavedSubj.get_subscriber().on_next(nullptr);
}
///	\brief load setttings from a rapidjson document
///	\param	doc	the rapidjson document that holds our settings
void PlayerSettings::Load(rapidjson::Document& doc)
{
	auto obj = doc.GetObject();
	if (obj.HasMember("PlayerSettings"))
	{
		auto member = obj["PlayerSettings"].GetObject();
		if (member.HasMember("LoadedFilepath"))
		{
			this->LoadedVideoFilepath = member["LoadedFilepath"].GetString();
		}
		if (member.HasMember("CurrentFrameId"))
		{
			this->CurrentFrameId = member["CurrentFrameId"].GetUint64();
		}
	}
	this->m_OnSettingsLoadedSubj.get_subscriber().on_next(nullptr);
}
///	\brief on settings saved event output
rxcpp::observable<void*> PlayerSettings::OnSettingsSaved()
{
	return this->m_OnSettingsSavedSubj.get_observable().as_dynamic();
}
///	\brief on settings loaded event output
rxcpp::observable<void*> PlayerSettings::OnSettingsLoaded()
{
	return this->m_OnSettingsLoadedSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu