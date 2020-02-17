#include <Core/SettingsRepo.h>

#include <filesystem>
#include <algorithm>
#include <fstream>

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>

#include <plog/Log.h>

namespace fusion {
///	\struct Impl
///	\brief SettingsRepo Implementation
struct SettingsRepo::Impl
{
	std::vector<settings_ptr_t> m_Settings;
	bool						m_SettingsSaved{ false };
	rxcpp::subjects::subject<void*> m_OnSettingsSavedSubject;
	rxcpp::subjects::subject<void*> m_OnSettingsLoadedSubject;
	/// Construction
	Impl() { }
};	///	!struct Impl
/// Construction
SettingsRepo::SettingsRepo()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
/// DEstruction
SettingsRepo::~SettingsRepo()
{
	if (!m_Impl->m_Settings.empty())
	{
		m_Impl->m_Settings.clear();
	}
}

void SettingsRepo::Init()
{

}

void SettingsRepo::RegisterSettings(settings_ptr_t s)
{
	if (std::find(m_Impl->m_Settings.begin(), m_Impl->m_Settings.end(), s) == m_Impl->m_Settings.end())
	{
		m_Impl->m_Settings.push_back(s);
	}
}

void SettingsRepo::UnregisterSettings(settings_ptr_t s)
{
	m_Impl->m_Settings.erase(
		std::remove_if(m_Impl->m_Settings.begin(), m_Impl->m_Settings.end(), [s](auto x) { 
			return x == s;
		}), m_Impl->m_Settings.end());
}

void SettingsRepo::Save(const std::string& filepath)
{
	using namespace rapidjson;
	std::ofstream out_fs(filepath);
	OStreamWrapper out_sw(out_fs);
	PrettyWriter<OStreamWrapper> writer(out_sw);

	writer.StartObject();
	for (auto& s : m_Impl->m_Settings)
		s->Save(writer);
	writer.EndObject();
	out_fs.close();
	std::string savePath = std::experimental::filesystem::path(filepath).parent_path().generic_string();
	m_Impl->m_SettingsSaved = true;
	m_Impl->m_OnSettingsSavedSubject.get_subscriber().on_next(nullptr);
}

void SettingsRepo::Load(const std::string& filepath)
{
	using namespace rapidjson;
	std::ifstream in_fs(filepath);
	IStreamWrapper in_sw(in_fs);
	Document doc;
	doc.ParseStream(in_sw);
	if (in_fs.is_open())
	{
		for (auto& s : m_Impl->m_Settings)
		{
			s->Load(doc);
		}
	}
	in_fs.close();
	m_Impl->m_OnSettingsLoadedSubject.get_subscriber().on_next(nullptr);
}


rxcpp::observable<void*> SettingsRepo::OnSettingsSaved()
{
	return m_Impl->m_OnSettingsSavedSubject.get_observable().as_dynamic();
}
rxcpp::observable<void*> SettingsRepo::OnSettingsLoaded()
{
	return m_Impl->m_OnSettingsLoadedSubject.get_observable().as_dynamic();
}

}	///	!namespace fusion