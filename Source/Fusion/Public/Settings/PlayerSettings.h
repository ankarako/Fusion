#ifndef	__FUSION_PUBLIC_SETTINGS_PLAYERSETTINGS_H__
#define __FUSION_PUBLIC_SETTINGS_PLAYERSETTINGS_H__

#include <Core/Settings.h>
#include <string>

namespace fu {
namespace fusion {
/// \class PlayerSettings
///	\brief just holds the loaded video's filepath
class PlayerSettings : public Settings<WriterType::PrettyWriter>
{
public:
	std::string	LoadedVideoFilepath;

	void Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer) override;
	void Load(rapidjson::Document& doc) override;
	rxcpp::observable<void*> OnSettingsSaved() override;
	rxcpp::observable<void*> OnSettingsLoaded() override;
private:
	rxcpp::subjects::subject<void*> m_OnSettingsLoadedSubj;
	rxcpp::subjects::subject<void*> m_OnSettingsSavedSubj;
};	///	!class PlayerSettings
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_SETTINGS_PLAYERSETTINGS_H__