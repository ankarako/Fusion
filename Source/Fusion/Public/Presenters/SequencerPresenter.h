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

class SequencerPresenter	: public app::Initializable
{
public:
	using view_ptr_t			= std::shared_ptr<SequencerView>;
	using wrepo_ptr_t			= std::shared_ptr<app::WidgetRepo>;
	using player_ptr_t			= std::shared_ptr<PlayerModel>;
	using perfcap_player_ptr_t	= std::shared_ptr<PerfcapPlayerModel>;

	SequencerPresenter(view_ptr_t view, wrepo_ptr_t wrepo, player_ptr_t player_model, perfcap_player_ptr_t perfcap_player);
	void Init() override;

private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class SequencerPresenter
}	///	!namesapce fusion
}	///	!namesapce fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_SEQUENCERVIEW_H__