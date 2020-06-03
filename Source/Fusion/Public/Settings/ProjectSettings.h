#ifndef __FUSION_PUBLIC_SETTINGS_PROJECTSETTINGS_H__
#define __FUSION_PUBLIC_SETTINGS_PROJECTSETTINGS_H__

#include <Core/Settings.h>
#include <string>

namespace fu {
namespace fusion {
class ProjectSettings : public Settings<WriterType::PrettyWriter>
{
public:
	std::string ProjectPath{ "" };
	std::string WorkspacePath{ "" };
	std::string ProjectName{ "" };

	void Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer) override;
	void Load(rapidjson::Document& doc) override;
	rxcpp::observable<void*> OnSettingsLoaded() override;
	rxcpp::observable<void*> OnSettingsSaved() override;
private:
	rxcpp::subjects::subject<void*> m_OnSettingsLoadedSubj;
	rxcpp::subjects::subject<void*> m_OnSettingsSavedSubj;
};
}	///	!namespace fusion
}	///	!namespace fu
#endif /// !__FUSION_PUBLIC_SETTINGS_PROJECTSETTINGS_H__
