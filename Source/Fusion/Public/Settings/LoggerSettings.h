#ifndef	__FUSION_PUBLIC_SETTINGS_LOGGERSETTINGS_H__
#define __FUSION_PUBLIC_SETTINGS_LOGGERSETTINGS_H__

#include <Core/Settings.h>

#include <string.h>

namespace fusion {
///
class LoggerSetings : public Settings<WriterType::PrettyWriter>, public Settings<WriterType::File>
{
public:
	std::string LogFilepath;

	void save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer) override;
	void load(rapidjson::Document& doc) override;
	void save(const std::string& filepath) override;
	void load(const std::string& filepath) override;	
};	///	!struct LoggerSettings
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_SETTINGS_LOGGERSETTINGS_H__