#ifndef __FUSION_PUBLIC_SEQUENCERVIEW_H__
#define __FUSION_PUBLIC_SEQUENCERVIEW_H__

#include <Widget.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>
#include <Core/SequenceItem.h>

namespace fu {
namespace app {
class FontManager;
}
namespace fusion {

class SequencerView	: public app::Widget
{
public:
	using fman_ptr_t = std::shared_ptr<app::FontManager>;

	SequencerView(fman_ptr_t fman);
	void Render() override;

	rxcpp::observer<SequenceItem>	SequencerItemFlowIn();
	rxcpp::observer<int>			CurrentFrameFlowIn();
	rxcpp::observer<void*>			CurrentFrameTickFlowIn();

	rxcpp::observable<void*>		OnPlayButtonClicked();
	rxcpp::observable<void*>		OnPauseButtonClicked();
	rxcpp::observable<void*>		OnStopButtonClicked();
	rxcpp::observable<void*>		OnSeekBackwardButtonClicked();
	rxcpp::observable<void*>		OnSeekForwardButtonClicked();

	rxcpp::observable<int>			OnTransportPositionChanged();
	rxcpp::observable<void*>		OnVideoStartPlayback();
	rxcpp::observable<void*>		OnAnimationStartPlayback();

	rxcpp::observable<void*>		OnTranslationClicked();
	rxcpp::observable<void*>		OnRotationClicked();
	rxcpp::observable<void*>		OnScaleClicked();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class SequencerView
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_SEQUENCERVIEW_H__