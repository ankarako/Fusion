#ifndef __FUSION_PUBLIC_PRESENTERS_SEQUENCERVIEW_H__
#define __FUSION_PUBLIC_PRESENTERS_SEQUENCERVIEW_H__

#include <Initializable.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace app {
class WidgetRepo;
}
namespace fusion {
class SequencerView;
class PlayerModel;
class PerfcapPlayerModel;
class RayTracingModel;
class VideoTracingModel;
class AnimationModel;
class Coordination;

class SequencerPresenter	: public app::Initializable
{
public:
	using view_ptr_t			= std::shared_ptr<SequencerView>;
	using wrepo_ptr_t			= std::shared_ptr<app::WidgetRepo>;
	using player_ptr_t			= std::shared_ptr<PlayerModel>;
	using perfcap_player_ptr_t	= std::shared_ptr<PerfcapPlayerModel>;
	using rt_model_ptr_t		= std::shared_ptr<RayTracingModel>;
	using vrt_model_ptr_t		= std::shared_ptr<VideoTracingModel>;
	using anim_model_ptr_t		= std::shared_ptr<AnimationModel>;
	using coord_ptr_t			= std::shared_ptr<Coordination>;

	SequencerPresenter(
		view_ptr_t view, 
		wrepo_ptr_t wrepo, 
		player_ptr_t player_model, 
		perfcap_player_ptr_t perfcap_player,
		rt_model_ptr_t rt_model,
		vrt_model_ptr_t vrt_model,
		anim_model_ptr_t anim_model,
		coord_ptr_t coord);
	void Init() override;

private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class SequencerPresenter
}	///	!namesapce fusion
}	///	!namesapce fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_SEQUENCERVIEW_H__