#ifndef	__FUSION_PUBLIC_VIEWS_PLAYERCONTROLLERVIEW_H__
#define __FUSION_PUBLIC_VIEWS_PLAYERCONTROLLERVIEW_H__

#include <Widget.h>
#include <spimpl.h>

namespace app {
class FontManager;
}
namespace fu {
namespace fusion {
class PlayerControllerView : public app::Widget
{
public:
	using fman_ptr_t = std::shared_ptr<app::FontManager>;
	/// Construction
	PlayerControllerView(fman_ptr_t fman);
	///	widget rendering
	void Render() override;

	rxcpp::observable<void*>	OnSeekBackwardButtonClicked();
	rxcpp::observable<void*>	OnPlayButtonClicked();
	rxcpp::observable<void*>	OnStopButtonClicked();
	rxcpp::observable<void*>	OnSeekForwardButtonClicked();
	rxcpp::observer<int>		FrameIdFlowIn();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PlayerControllerView
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_VIEWS_PLAYERCONTROLLERVIEW_H__