#ifndef	__MVT_PUBLIC_PRESENTERS_MULTIVIEWPLAYERPRESENTER_H__
#define __MVT_PUBLIC_PRESENTERS_MULTIVIEWPLAYERPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace mvt {
class MultiViewPlayerModel;
class PerformanceImportModel;
///	\class MultiViewPlayerPresenter
///	\brief interacts with multiview player
class MultiViewPlayerPresenter : public app::Initializable
{
public:
	using perf_import_model_ptr_t 	= std::shared_ptr<PerformanceImportModel>;
	using player_model_ptr_t 		= std::shared_ptr<MultiViewPlayerModel>;
	/// Construction
	///
	MultiViewPlayerPresenter(player_model_ptr_t model, perf_import_model_ptr_t perf_import_model);
	/// \brief presenter initialization
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class MultiViewPlayerPresenter
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_PRESENTERS_MULTIVIEWPLAYERPRESENTER_H__