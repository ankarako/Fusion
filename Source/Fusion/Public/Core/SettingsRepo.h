#ifndef __FUSION_PUBLIC_CORE_SETTINGSREPO_H__
#define __FUSION_PUBLIC_CORE_SETTINGSREPO_H__

#include <Initializable.h>
#include <Core/Settings.h>

#include <spimpl.h>

#include <vector>
#include <memory>
#include <rxcpp/rx.hpp>

namespace fusion {
///	\class SettingsRepo
///	\brief a simple settings repository that holds settings
class SettingsRepo final : public Settings<WriterType::File>, public app::Initializable
{
public:
	using settings_ptr_t = std::shared_ptr<Settings<WriterType::PrettyWriter>>;
	/// Construction
	SettingsRepo();
	/// Destruction
	~SettingsRepo();
	/// Initialize
	void Init() override;
	///	\brief register settings
	void RegisterSettings(settings_ptr_t settings);
	///	\brief unregister settings
	void UnregisterSettings(settings_ptr_t settings);
	///	\brief save all registerd settings to file
	void Save(const std::string& filepath) override;
	///	\brief load settings file
	void Load(const std::string& filepath) override;
	///	\brief check if the settings are saved
	bool SettingsSaved() const;
	///	\observables
	rxcpp::observable<void*> OnSettingsSaved() override;
	rxcpp::observable<void*> OnSettingsLoaded() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class SettingsRepo
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_CORE_SETTINGSREPO_H__