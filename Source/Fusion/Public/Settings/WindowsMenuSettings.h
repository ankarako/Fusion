#ifndef __FUSION_PUBLIC_SETTINGS_WINDOWSMENUSETTINGS_H__
#define __FUSION_PUBLIC_SETTINGS_WINDOWSMENUSETTINGS_H__

#include <Core/Settings.h>

namespace fu {
namespace fusion {
class WindowsMenuSettings : public Settings<WriterType::PrettyWriter>
{
public:
	bool NormalsWindowVisible{ false };
	bool IlluminationWindowVisible{ false };
	bool ViewpotOptionsVisible{ false };
	
	void Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer) override;
	void Load(rapidjson::Document& doc) override;
	rxcpp::observable<void*> OnSettingsSaved() override;
	rxcpp::observable<void*> OnSettingsLoaded() override;
private:
	rxcpp::subjects::subject<void*> m_OnSettingsLoadedSubj;
	rxcpp::subjects::subject<void*> m_OnSettingsSavedSubj;
};	///	!class WindowsMenuSettings
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_SETTINGS_WINDOWSMENUSETTINGS_H__