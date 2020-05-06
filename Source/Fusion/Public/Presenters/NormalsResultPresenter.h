#ifndef __FUSION_PUBLIC_NORMALSRESULTPRESENTER_H__
#define __FUSION_PUBLIC_NORMALSRESULTPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace fusion {

class NormalsResultView;
class NormalsEstimationModel;
class Coordination;
class RayTracingModel;

class NormalsResultPresenter : public app::Initializable
{
public:
	using view_ptr_t = std::shared_ptr<NormalsResultView>;
	using model_ptr_t = std::shared_ptr<NormalsEstimationModel>;
	using coord_ptr_t = std::shared_ptr<Coordination>;
	using rt_model_ptr_t = std::shared_ptr<RayTracingModel>;
	/// Construction
	NormalsResultPresenter(view_ptr_t view, model_ptr_t model, coord_ptr_t coord, rt_model_ptr_t rt_model);
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class NormalsReultPresenter
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_NORMALSRESULTPRESENTER_H__