#ifndef	__FUSION_PUBLIC_VIEWS_PLAYERVIEWPORTVIEW_H__
#define __FUSION_PUBLIC_VIEWS_PLAYERVIEWPORTVIEW_H__

#include <Buffer.h>
#include <Initializable.h>
#include <Widget.h>
#include <rxcpp/rx.hpp>
#include <spimpl.h>
#include <GL/gl3w.h>

namespace fu {
namespace app {
class FontManager;
}
namespace fusion {
class Coordination;
///	\class PlayerViewportView
class PlayerViewportView : public app::Widget, public app::Initializable
{
public:
	using coord_ptr_t = std::shared_ptr<Coordination>;
	using fman_ptr_t = std::shared_ptr<app::FontManager>;
	/// Construction
	PlayerViewportView(fman_ptr_t fman, coord_ptr_t coord);
	/// brief initialize the widget
	void Init() override;
	///	widget render
	void Render() override;
	///	input events
	///	frame iput
	rxcpp::observer<BufferCPU<uchar4>> FrameFlowIn();
	///	frame width input
	rxcpp::observer<int> FrameWidthFlowIn();
	/// frame height input
	rxcpp::observer<int> FrameHeightFlowIn();
	///	frame size input
	rxcpp::observer<uint2> FrameSizeFlowIn();
	/// output events
	rxcpp::observable<float>	OnViewportWidthChanged();
	rxcpp::observable<float>	OnViewportHeightChanged();
	rxcpp::observable<GLuint>	PixelBufferHandleFlowOut();
private:	
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PlayerViewportView
}	///	!namespace fusion
}
#endif	///	!__FUSION_PUBLIC_VIEWS_PLAYERVIEWPORTVIEW_H__