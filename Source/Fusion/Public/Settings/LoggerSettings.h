#ifndef	__FUSION_PUBLIC_SETTINGS_LOGGERSETTINGS_H__
#define __FUSION_PUBLIC_SETTINGS_LOGGERSETTINGS_H__

#include <Core/Settings.h>

#include <string.h>

namespace fusion {
///
class LoggerSettings : public Settings<WriterType::PrettyWriter>, public Settings<WriterType::File>
{
public:
	std::string LogFilepath;

	void Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer) override;
	void Load(rapidjson::Document& doc) override;
	void Save(const std::string& filepath) override;
	void Load(const std::string& filepath) override;	
};	///	!struct LoggerSettings
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_SETTINGS_LOGGERSETTINGS_H__