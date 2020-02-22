#ifndef	__FUSION_PUBLIC_VIEWS_PLAYERVIEWPORTVIEW_H__
#define __FUSION_PUBLIC_VIEWS_PLAYERVIEWPORTVIEW_H__

#include <Widget.h>
#include <spimpl.h>

namespace app {
class FontManager;
}

namespace fu {
namespace fusion {
///	\class PlayerViewportView
class PlayerViewportView : public app::Widget
{
public:
	using fman_ptr_t = std::shared_ptr<app::FontManager>;
	/// Construction
	PlayerViewportView(fman_ptr_t fman);
	///	widget render
	void Render() override;
	/// output events
	rxcpp::observable<float>	OnViewportWidthChanged();
	rxcpp::observable<float>	OnViewportHeightChanged();
private:	
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PlayerViewportView
}	///	!namespace fusion
}
#endif	///	!__FUSION_PUBLIC_VIEWS_PLAYERVIEWPORTVIEW_H__