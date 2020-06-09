#ifndef	__FUSION_PUBLIC_ILLUMINATIONESTIMATIONSETTINGS_H__
#define __FUSION_PUBLIC_ILLUMINATIONESTIMATIONSETTINGS_H__

#include <Core/Settings.h>
#include <string>

namespace fu {
namespace fusion {

class IlluminationEstimationSettings : public Settings<WriterType::PrettyWriter>
{
public:
	std::string IlluminationFilepathEXR{ "" };

	void Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer) override;
	void Load(rapidjson::Document& doc) override;
	rxcpp::observable<void*> OnSettingsSaved() override;
	rxcpp::observable<void*> OnSettingsLoaded() override;
private:
	rxcpp::subjects::subject<void*> m_OnSettingsSavedSubj;
	rxcpp::subjects::subject<void*> m_OnSettingsLoadedSubj;
};	///	!class IlluminationEstimationSettings
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_ILLUMINATIONESTIMATIONSETTINGS_H__