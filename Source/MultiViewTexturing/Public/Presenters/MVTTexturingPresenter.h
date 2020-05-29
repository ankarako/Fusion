#ifndef	__MVT_PUBLIC_PRESENTERS_MVTTEXTTURINGPRESENTER_H__
#define __MVT_PUBLIC_PRESENTERS_MVTTEXTTURINGPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace mvt {
class MVTModel;
class PerformanceImportModel;
class ViewportTracingModel;
class MultiViewPlayerModel;
///	\class MVTPresenter
///	\brief MVT interaction
class MVTPresenter : public app::Initializable
{
public:
	using model_ptr_t				= std::shared_ptr<MVTModel>;
	using perf_import_model_ptr_t	= std::shared_ptr<PerformanceImportModel>;
	using viewport_model_ptr_t		= std::shared_ptr<ViewportTracingModel>;
	using mv_player_model_ptr_t		= std::shared_ptr<MultiViewPlayerModel>;

	MVTPresenter(model_ptr_t model, perf_import_model_ptr_t perf_import_model, viewport_model_ptr_t viewport_model, mv_player_model_ptr_t mv_player_model);
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class MVTPresenter
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_PRESENTERS_MVTTEXTTURINGPRESENTER_H__