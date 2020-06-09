#include <Settings/FuImportSettings.h>

namespace fu {
namespace fusion {

void FuImportSettings::Save(rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer)
{
	using namespace rapidjson;
	writer.Key("FuImportSettings");
	writer.StartObject();
	{
		writer.Key("TemplateMeshFilepath");
		writer.String(this->TemplateMeshFilepath.c_str());
		writer.Key("SkeletonFilepath");
		writer.String(this->SkeletonFilepath.c_str());
		writer.Key("SkinDataFilepath");
		writer.String(this->SkinDataFilepath.c_str());
		writer.Key("TrackedDataFilepath");
		writer.String(this->TrackedParamsFilepath.c_str());
		writer.Key("TextureFilepath");
		writer.String(this->TextureVideoFilepath.c_str());
		writer.Key("Name");
		writer.String(this->Name.c_str());
	}
	writer.EndObject();
	this->m_OnSettingsSavedSubj.get_subscriber().on_next(nullptr);
}

void FuImportSettings::Load(rapidjson::Document& doc)
{
	auto obj = doc.GetObject();
	if (obj.HasMember("FuImportSettings"))
	{
		auto set = obj["FuImportSettings"].GetObject();
		if (set.HasMember("TemplateMeshFilepath"))
		{
			this->TemplateMeshFilepath = set["TemplateMeshFilepath"].GetString();
		}
		if (set.HasMember("SkeletonFilepath"))
		{
			this->SkeletonFilepath = set["SkeletonFilepath"].GetString();
		}
		if (set.HasMember("SkinDataFilepath"))
		{
			this->SkinDataFilepath = set["SkinDataFilepath"].GetString();
		}
		if (set.HasMember("TrackedDataFilepath"))
		{
			this->TrackedParamsFilepath = set["TrackedDataFilepath"].GetString();
		}
		if (set.HasMember("TextureFilepath"))
		{
			this->TextureVideoFilepath = set["TextureFilepath"].GetString();
		}
		if (set.HasMember("Name"))
		{
			this->Name = set["Name"].GetString();
		}
		this->m_OnSettingsLoadedSubj.get_subscriber().on_next(nullptr);
	}
}
rxcpp::observable<void*> FuImportSettings::OnSettingsSaved()
{
	return this->m_OnSettingsSavedSubj.get_observable().as_dynamic();
}
rxcpp::observable<void*> FuImportSettings::OnSettingsLoaded()
{
	return this->m_OnSettingsLoadedSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu