#ifndef	__FUSION_PUBLIC_SETTINGS_NORMALESTIMATIONSETTINGS_H__
#define __FUSION_PUBLIC_SETTINGS_NORMALESTIMATIONSETTINGS_H__

#include <Core/Settings.h>
#include <string>

namespace fu {
namespace fusion {
class NormalEstimationSettings : public Settings<WriterType::PrettyWriter>
{
public:
	::std::string NormalFilepathEXR{ "" };
	::std::string NormalFilepathPNG{ "" };

	void Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer) override;
	void Load(rapidjson::Document& doc) override;
	rxcpp::observable<void*> OnSettingsLoaded() override;
	rxcpp::observable<void*> OnSettingsSaved() override;
private:
	rxcpp::subjects::subject<void*> m_OnSettingsLoadedSubj;
	rxcpp::subjects::subject<void*> m_OnSettingsSavedSubj;
};	///	!class NormalEstimationSettings
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_SETTINGS_NORMALESTIMATIONSETTINGS_H__