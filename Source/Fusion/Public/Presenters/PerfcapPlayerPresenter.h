#ifndef __FUSION_PUBLIC_PRESENTERS_PERFCAPPLAYERPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_PERFCAPPLAYERPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {

class PerfcapPlayerModel;
class RayTracingModel;
class SequencerView;
class FuImportModel;
class VideoTracingModel;
class Coordination;

class PerfcapPlayerPresenter	: public app::Initializable
{
public:
	using model_ptr_t 		= std::shared_ptr<PerfcapPlayerModel>;
	using rt_model_ptr_t	= std::shared_ptr<RayTracingModel>;
	using seq_view_ptr_t	= std::shared_ptr<SequencerView>;
	using fuimport_model_ptr_t = std::shared_ptr<FuImportModel>;
	using vrt_model_ptr_t	= std::shared_ptr<VideoTracingModel>;
	using coord_ptr_t		= std::shared_ptr<Coordination>;
	/// Construction
	PerfcapPlayerPresenter(
		model_ptr_t model, 
		rt_model_ptr_t rt_model, 
		seq_view_ptr_t seq_view, 
		fuimport_model_ptr_t fuimport_model,
		vrt_model_ptr_t vrt_model,
		coord_ptr_t coord);
	///
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PerfcapPlayerPresenter
}
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_PERFCAPPLAYERPRESENTER_H__