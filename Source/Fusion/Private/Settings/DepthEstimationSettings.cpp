#include <Settings/DepthEstimationSettings.h>

namespace fu {
namespace fusion {
/// DepthEstimationSettings Implementation
/// \brief save settings to a pretty writer
///	\param	writer	the writer to write our settings into
void DepthEstimationSettings::Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer)
{
	writer.Key("DepthEstimationSettings");
	writer.StartObject();
	writer.Key("DepthFilepathEXR");
	writer.String(this->DepthFilepathEXR.c_str());
	writer.Key("PointCloudFilepathPLY");
	writer.String(this->PointCloudFilepathPLY.c_str());
	writer.EndObject();
	this->m_OnSettingsSavedSubj.get_subscriber().on_next(nullptr);
}
///	\brief load settings from a rapidjson document
///	\param	doc	the document to load our settings from
void DepthEstimationSettings::Load(rapidjson::Document& doc)
{
	auto& obj = doc.GetObject();
	if (obj.HasMember("DepthEstimationSettings"))
	{
		auto& member = obj["DepthEstimationSettings"].GetObject();
		if (member.HasMember("DepthFilepathEXR"))
		{
			this->DepthFilepathEXR = member["DepthFilepathEXR"].GetString();
		}
		if (member.HasMember("PointCloudFilepathPLY"))
		{
			this->PointCloudFilepathPLY = member["PointCloudFilepathPLY"].GetString();
		}
	}
	this->m_OnSettingsLoadedSubj.get_subscriber().on_next(nullptr);
}

rxcpp::observable<void*> DepthEstimationSettings::OnSettingsLoaded()
{
	return m_OnSettingsLoadedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> DepthEstimationSettings::OnSettingsSaved()
{
	return m_OnSettingsSavedSubj.get_observable().as_dynamic();
}
}	/// !namespace fusion
}	///	!namespace fu