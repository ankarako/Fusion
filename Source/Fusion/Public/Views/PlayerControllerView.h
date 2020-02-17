#ifndef	__FUSION_PUBLIC_VIEWS_PLAYERCONTROLLERVIEW_H__
#define __FUSION_PUBLIC_VIEWS_PLAYERCONTROLLERVIEW_H__

#include <Widget.h>

#include <spimpl.h>

namespace fusion {
class PlayerControllerView : public app::Widget
{
public:
	/// Construction
	PlayerControllerView();
	///	widget rendering
	void Render() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PlayerControllerView
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_VIEWS_PLAYERCONTROLLERVIEW_H__