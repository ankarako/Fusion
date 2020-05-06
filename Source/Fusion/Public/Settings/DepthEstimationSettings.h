#ifndef	__FUSION_PUBLIC_SETTINGS_DEPTHESTIMATIONSETTINGS_H__
#define __FUSION_PUBLIC_SETTINGS_DEPTHESTIMATIONSETTINGS_H__

#include <Core/Settings.h>
#include <string>

namespace fu {
namespace fusion {
class DepthEstimationSettings : public Settings<WriterType::PrettyWriter>
{
public:
	std::string DepthFilepathEXR{ " " };
	std::string PointCloudFilepathPLY{ " " };

	void Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer) override;
	void Load(rapidjson::Document& doc) override;
	rxcpp::observable<void*> OnSettingsLoaded() override;
	rxcpp::observable<void*> OnSettingsSaved() override;
private:
	rxcpp::subjects::subject<void*> m_OnSettingsLoadedSubj;
	rxcpp::subjects::subject<void*> m_OnSettingsSavedSubj;
};	///	!class DepthEstimationSettings
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_SETTINGS_DEPTHESTIMATIONSETTINGS_H__