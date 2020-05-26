#ifndef __MVT_PUBLIC_PRESENTERS_PERFORMANCEIMPORTPRESENTER_H__
#define __MVT_PUBLIC_PRESENTERS_PERFORMANCEIMPORTPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace mvt {
class PerformanceImportModel;
class ViewportTracingModel;
class Coordination;
///	\class PerformanceImportPresenter
///	\brief connects Performance importing with associated models
class PerformanceImportPresenter : public app::Initializable
{
public:
	using model_ptr_t 			= std::shared_ptr<PerformanceImportModel>;
	using view_rt_model_ptr_t 	= std::shared_ptr<ViewportTracingModel>;
	using coord_ptr_t 			= std::shared_ptr<Coordination>;
	/// Construction
	PerformanceImportPresenter(model_ptr_t model, view_rt_model_ptr_t view_rt_model, coord_ptr_t coord);
	/// \brief presenter initialization
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PerformanceImportPresenter
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_PRESENTERS_PERFORMANCEIMPORTPRESENTER_H__