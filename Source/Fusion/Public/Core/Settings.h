#ifndef __FUSION_PUBLIC_CORE_SETTNGS_H__
#define __FUSION_PUBLIC_CORE_SETTNGS_H__

#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/document.h>
#include <rxcpp/rx.hpp>

#include <string>

namespace fusion {
///	\enum WriterType
///	\brief used for specializing settings classes
///	File: settings can write to files
///	PrettyWriter: settings can write to a Document
enum class WriterType : unsigned char
{
	Unknown = 0,
	File,
	PrettyWriter
};	///	!enum WriterType
///	\class Settings
///	\brief Empty settings specialization
template <WriterType Type = WriterType::Unknown>
class Settings { };
///	\class Settings
///	\brief File specialization
/// This are top-level settings that can write to files on disk
template <>
class Settings<WriterType::File>
{
public:
	virtual void Save(const std::string& filepath) = 0;
	virtual void Load(const std::string& filepath) = 0;
	virtual rxcpp::observable<void*> OnSettingsLoaded() { return rxcpp::observable<void*>(); }
	virtual rxcpp::observable<void*> OnSettingsSaved() { return rxcpp::observable<void*>(); }
};	///	!class Settings
///	\class Settings
///	\brief pretty writer specialization
template <>
class Settings<WriterType::PrettyWriter>
{
public:
	virtual void Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer) = 0;
	virtual void Load(rapidjson::Document& doc) = 0;
	virtual rxcpp::observable<void*> OnSettingsLoaded() { return rxcpp::observable<void*>(); }
	virtual rxcpp::observable<void*> OnSettingsSaved() { return rxcpp::observable<void*>(); }
};	///	!class Settings
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_CORE_SETTNGS_H__