#include <Settings/IlluminationEstimationSettings.h>

namespace fu {
namespace fusion {


void IlluminationEstimationSettings::Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer)
{
	writer.Key("IlluminationEstimationSettings");
	writer.StartObject();
	writer.Key("IlluminationFilepathEXR");
	writer.String(this->IlluminationFilepathEXR.c_str());
	writer.EndObject();
	m_OnSettingsSavedSubj.get_subscriber().on_next(nullptr);
}

void IlluminationEstimationSettings::Load(rapidjson::Document& doc)
{
	auto& obj = doc.GetObject();
	if (obj.HasMember("IlluminationEstimationSettings"))
	{
		auto& member = doc["IlluminationEstimationSettings"].GetObject();
		if (member.HasMember("IlluminationFilepathEXR"))
		{
			this->IlluminationFilepathEXR = member["IlluminationFilepathEXR"].GetString();
		}
		m_OnSettingsLoadedSubj.get_subscriber().on_next(nullptr);
	}
}

rxcpp::observable<void*> IlluminationEstimationSettings::OnSettingsSaved()
{
	return m_OnSettingsSavedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> IlluminationEstimationSettings::OnSettingsLoaded()
{
	return m_OnSettingsLoadedSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu