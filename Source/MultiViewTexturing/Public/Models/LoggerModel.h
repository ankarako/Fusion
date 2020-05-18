#ifndef	__MVT_PUBLIC_MODELS_LOGGERMODEL_H__
#define __MVT_PUBLIC_MODELS_LOGGERMODEL_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace mvt {
///	\class LoggerModel
///	\brief enables console logging
class LoggerModel : public app::Initializable
{
public:
	///	Construction
	///	\brief no dependencies
	LoggerModel();
	///	\brief initialize the model
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class LoggerModel
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_MODELS_LOGGERMODEL_H__