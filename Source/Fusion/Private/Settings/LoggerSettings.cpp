#include <Settings/LoggerSettings.h>

#include <fstream>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>

namespace fusion {

void LoggerSettings::Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer)
{
	writer.Key("Logger");
	writer.StartObject();
	writer.Key("LogFilepath");
	writer.String(LogFilepath.c_str());
	writer.EndObject();
}

void LoggerSettings::Load(rapidjson::Document& doc)
{
	auto obj = doc.GetObject();
	if (obj.HasMember("Logger"))
	{
		LogFilepath = obj["Logger"]["LogFilepath"].GetString();
	}
}

void LoggerSettings::Save(const std::string& filepath)
{
	using namespace rapidjson;
	std::ofstream out_fs(filepath);
	OStreamWrapper out_sw(out_fs);
	PrettyWriter<OStreamWrapper> writer(out_sw);
	writer.StartObject();
	writer.Key("LogFilepath");
	writer.String(LogFilepath.c_str());
	writer.EndObject();
	out_fs.close();
}

void LoggerSettings::Load(const std::string& filepath)
{
	using namespace rapidjson;
	std::ifstream in_fs(filepath);
	if (!in_fs.is_open())
		return;

	IStreamWrapper in_Sw(in_fs);
	Document doc;
	doc.ParseStream(in_sw);

	auto obj = doc.GetObject();
	if (obj.HasMember("LogFilepath"))
	{
		LogFilepath = obj["LogFilepath"].GetString();
	}
	in_fs.close();
}
}	///	!namespace fusion