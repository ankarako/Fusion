#ifndef	__FUSION_PUBLIC_VIEWS_PLAYERVIEWPORTVIEW_H__
#define __FUSION_PUBLIC_VIEWS_PLAYERVIEWPORTVIEW_H__

#include <Widget.h>
#include <spimpl.h>

namespace fusion {
///	\class PlayerViewportView
class PlayerViewportView : public app::Widget
{
public:
	/// Construction
	PlayerViewportView();
	///	widget render
	void Render() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PlayerViewportView
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_VIEWS_PLAYERVIEWPORTVIEW_H__