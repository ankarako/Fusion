#ifndef __FUSION_PUBLIC_SETTINGS_OMNICONNECTSETTINGS_H__
#define __FUSION_PUBLIC_SETTINGS_OMNICONNECTSETTINGS_H__

#include <Core/Settings.h>
#include <string>

namespace fu {
namespace fusion {
///	\class OmniconnectSettings
class OmniconnectSettings : public Settings<WriterType::PrettyWriter>
{
public:
	std::string VideoListResponseFilepath{ "" };
	int SelectedVideoListIndex{ -1 };
	void Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer) override;
	void Load(rapidjson::Document& doc) override;
	rxcpp::observable<void*> OnSettingsSaved() override;
	rxcpp::observable<void*> OnSettingsLoaded() override;
private:
	rxcpp::subjects::subject<void*> m_OnSettingsLoadedSubj;
	rxcpp::subjects::subject<void*> m_OnSettingsSavedSubj;
};
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_SETTINGS_OMNICONNECTSETTINGS_H__