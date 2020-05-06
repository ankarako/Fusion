#include <Settings/RayTracingViewportSettings.h>

namespace fu {
namespace fusion {
/// RaytRacingViewportSettings Implementation
void RayTracingViewportSettings::Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer)
{
	writer.Key("RayTracingViewportSettings");
	writer.StartObject();
	writer.Key("CeilingCulling");
	writer.Double(this->CeilingCulling);
	writer.Key("PointSize");
	writer.Double(this->PointSize);
	writer.EndObject();
	this->m_OnSettingsSavedSubj.get_subscriber().on_next(nullptr);
}

void RayTracingViewportSettings::Load(rapidjson::Document& doc)
{
	auto& obj = doc.GetObject();
	if (obj.HasMember("RayTracingViewportSettings"))
	{
		auto& member = obj["RayTracingViewportSettings"].GetObject();
		if (member.HasMember("CeilingCulling"))
		{
			this->CeilingCulling = member["CeilingCulling"].GetDouble();
		}
		if (member.HasMember("PointSize"))
		{
			this->PointSize = member["PointSize"].GetDouble();
		}
		this->m_OnSettingsLoadedSubj.get_subscriber().on_next(nullptr);
	}
}

rxcpp::observable<void*> RayTracingViewportSettings::OnSettingsSaved()
{
	return m_OnSettingsSavedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> RayTracingViewportSettings::OnSettingsLoaded()
{
	return m_OnSettingsLoadedSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu