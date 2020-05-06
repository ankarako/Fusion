#include <Settings/NormalEstimationSettings.h>

namespace fu {
namespace fusion {
/// NormalEstimationSettings Implementation
///	\brief save settings to a pretty writer
///	\param	writer	the writer to write the settings into
void NormalEstimationSettings::Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer)
{
	writer.Key("NormalEstimationSettings");
	writer.StartObject();
	writer.Key("FilepathEXR");
	writer.String(this->NormalFilepathEXR.c_str());
	writer.Key("FilepathPNG");
	writer.String(this->NormalFilepathPNG.c_str());
	writer.EndObject();
	this->m_OnSettingsSavedSubj.get_subscriber().on_next(nullptr);
}
///	\brief load settings from a rapidjson document
///	\param	doc	the document containting our settings
void NormalEstimationSettings::Load(rapidjson::Document& doc)
{
	auto& obj = doc.GetObject();
	if (obj.HasMember("NormalEstimationSettings"))
	{
		auto& member = obj["NormalEstimationSettings"].GetObject();
		if (member.HasMember("FilepathEXR"))
		{
			this->NormalFilepathEXR = member["FilepathEXR"].GetString();
		}
		if (member.HasMember("FilepathPNG"))
		{
			this->NormalFilepathPNG = member["FilepathPNG"].GetString();
		}
		this->m_OnSettingsLoadedSubj.get_subscriber().on_next(nullptr);
	}
	
}

rxcpp::observable<void*> NormalEstimationSettings::OnSettingsSaved()
{
	return m_OnSettingsSavedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> NormalEstimationSettings::OnSettingsLoaded()
{
	return m_OnSettingsLoadedSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu