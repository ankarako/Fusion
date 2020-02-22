#ifndef	__FUSION_PUBLIC_MODELS_LOGGER_MODEL_H__
#define __FUSION_PUBLIC_MODELS_LOGGER_MODEL_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace fusion {
///
class SettingsRepo;
///	\class LoggerModel
///	\brief Logger class
class LoggerModel : public app::Initializable
{
public:
	using srepo_ptr_t = std::shared_ptr<SettingsRepo>;
	/// Construction
	LoggerModel(srepo_ptr_t srepo);
	~LoggerModel() = default;
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class LoggerModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_LOGGER_MODEL_H__