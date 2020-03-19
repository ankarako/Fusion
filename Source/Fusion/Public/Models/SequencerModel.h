#ifndef __FUSION_PUBLIC_MODELS_SEQUENCERMODEL_H__
#define __FUSION_PUBLIC_MODELS_SEQUENCERMODEL_H__

#include <Initializable.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {
///
class PlayerModel;
// class AnimationModel;

class SequencerModel : public app::Initializable
{
public:
	using player_model_ptr_t = std::shared_ptr<PlayerModel>;
	SequencerModel(player_model_ptr_t player_model);

	void Init() override;
	void Start();
	void Pause();
	void Stop();
	
	rxcpp::observer<size_t> PlayerTrackPositionFlowIn();
	rxcpp::observer<size_t> AnimationTrackPositionFlowIn();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class SequencerModel
}
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_SEQUENCERMODEL_H__