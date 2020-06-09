#ifndef __FUSION_PUBLIC_SETTINGS_FUMPORTSETTINGS_H__
#define __FUSION_PUBLIC_SETTINGS_FUMPORTSETTINGS_H__

#include <Core/Settings.h>
#include <string>

namespace fu {
namespace fusion {

class FuImportSettings : public Settings<WriterType::PrettyWriter>
{
public:
	std::string TemplateMeshFilepath{ "" };
	std::string SkeletonFilepath{ "" };
	std::string SkinDataFilepath{ "" };
	std::string TrackedParamsFilepath{ "" };
	std::string TextureVideoFilepath{ "" };
	std::string Name{ "" };

	void Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer) override;
	void Load(rapidjson::Document& doc) override;
	rxcpp::observable<void*> OnSettingsSaved() override;
	rxcpp::observable<void*> OnSettingsLoaded() override;
private:
	rxcpp::subjects::subject<void*> m_OnSettingsLoadedSubj;
	rxcpp::subjects::subject<void*> m_OnSettingsSavedSubj;
};	///	!class FuImportSettings
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_SETTINGS_FUIMPORTSETTINGS_H__