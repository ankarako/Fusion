#ifndef	__FUSION_PUBLIC_PRESENTERS_TRANSFORMATIONPRESENTERS_H__
#define __FUSION_PUBLIC_PRESENTERS_TRANSFORMATIONPRESENTERS_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;
}
namespace fusion {

class SequencerView;
class TransformationView;
class RayTracingModel;
class VideoTracingModel;

class TransformationsPresenter : public app::Initializable
{
public:
	using seq_view_ptr_t 	= std::shared_ptr<SequencerView>;
	using trans_view_ptr_t 	= std::shared_ptr<TransformationView>;
	using rt_model_ptr_t 	= std::shared_ptr<RayTracingModel>;
	using vrt_model_ptr_t 	= std::shared_ptr<VideoTracingModel>;
	using wrepo_ptr_t		= std::shared_ptr<app::WidgetRepo>;

	TransformationsPresenter(seq_view_ptr_t seq_view, trans_view_ptr_t trans_view, rt_model_ptr_t rt_model, vrt_model_ptr_t vrt_model, wrepo_ptr_t wrepo);
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_TRANSFORMATIONPRESENTERS_H__