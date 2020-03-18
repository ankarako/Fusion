#ifndef	__FUSION_PUBLIC_SETTINGS_RAYTRACINGSETTINGS_H__
#define __FUSION_PUBLIC_SETTINGS_RAYTRACINGSETTINGS_H__

#include <Core/Settings.h>
#include <vector>
#include <string>

namespace fu {
namespace fusion {

class RayTracingSettings : public Settings<WriterType::PrettyWriter>
{
public:
	std::vector<std::string> LoadedFiles;

	void Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer) override;
	void Load(rapidjson::Document& doc) override;
	rxcpp::observable<void*> OnSettingsSaved() override;
	rxcpp::observable<void*> OnSettingsLoaded() override;
private:
	rxcpp::subjects::subject<void*> m_OnSettingsLoadedSubj;
	rxcpp::subjects::subject<void*> m_OnSettingsSavedSubj;
};	///	!class RayTracingSettings
}	///	!namesapce fusion
}	/// !namespace gfu
#endif	///	!__FUSION_PUBLIC_SETTINGS_RAYTRACINGSETTINGS_H__