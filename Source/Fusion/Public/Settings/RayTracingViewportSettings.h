#ifndef	__FUSION_PUBLIC_SETTINGS_RAYTRACINGVIEWPORTSETTINGS_H__
#define __FUSION_PUBLIC_SETTINGS_RAYTRACINGVIEWPORTSETTINGS_H__

#include <Core/Settings.h>

namespace fu {
namespace fusion {

class RayTracingViewportSettings : public Settings<WriterType::PrettyWriter>
{
public:
	float CeilingCulling{};
	float PointSize{};

	void Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer) override;
	void Load(rapidjson::Document& doc) override;

	rxcpp::observable<void*> OnSettingsSaved() override;
	rxcpp::observable<void*> OnSettingsLoaded() override;
private:
	rxcpp::subjects::subject<void*> m_OnSettingsSavedSubj;
	rxcpp::subjects::subject<void*> m_OnSettingsLoadedSubj;
};	///	!class RayTracingSettings
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_SETTINGS_RAYTRACINGVIEWPORTSETTINGS_H__