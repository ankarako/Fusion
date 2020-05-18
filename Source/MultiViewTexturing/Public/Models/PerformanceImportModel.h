#ifndef __MVT_PUBLIC_MODELS_PERFORMACEIMPORTMODEL_H__
#define __MVT_PUBLIC_MODELS_PERFORMACEIMPORTMODEL_H__

#include <Initializable.h>
#include <Destroyable.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>
#include <string>

namespace fu {
namespace mvt {
///	\class PerformanceImportModel
///	\brief imports performance files
class PerformanceImportModel : public app::Initializable, public app::Destroyable
{
public:
	/// Construction
	///	\brief no dependencies
	PerformanceImportModel();
	/// \brief Initialize the model
	void Init() override;
	///	\brief destroy the model
	void Destroy() override;
	/// inputs
	///	\brief filepath input for importing
	rxcpp::observer<std::string> ImportFilepathFlowIn();
	/// outputs	
	// rxcpp::observable<>
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PerformanceImportModel
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_MODELS_PERFORMACEIMPORTMODEL_H__