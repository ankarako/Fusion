#ifndef __MVT_PUBLIC_VIEWS_VIEWPORTVIEW_H__
#define __MVT_PUBLIC_VIEWS_VIEWPORTVIEW_H__

#include <Widget.h>
#include <Initializable.h>
#include <spimpl.h>
#include <Buffer.h>
#include <rxcpp/rx.hpp>
#include <array>

namespace fu {
namespace mvt {
///	\class ViewportView
///	\brief Viewport Window rendering and interaction
class ViewportView : public app::Widget, public app::Initializable
{
public:
	/// \typedef viewport_size_t
	///	\brief	the width and height of the viewport
	using viewport_size_t	= std::array<int, 2>;
	using mouse_pos_t		= std::array<int, 2>;
	using mat_t				= std::array<float, 16>;
	using trans_vec_t		= std::array<float, 3>;
	/// Construction
	///	\brief default constructor
	///	no dependencies
	ViewportView();
	/// \brief view initialization
	void Init() override;
	///	\brief window rendering
	void Render() override;
	/// Input events
	/// Output events
	rxcpp::observable<viewport_size_t> 		ViewportSizeFlowOut();
	rxcpp::observable<void*> 				OnCloseButtonClicked();
	rxcpp::observer<BufferCPU<uchar4>>		FrameBufferFlowIn();
	rxcpp::observable<mat_t>				RotationTransformFlowOut();
	rxcpp::observable<trans_vec_t>			TranslationVectorFlowOut();

	rxcpp::observable<void*>				LeftMouseButtonDownFlowOut();
	rxcpp::observable<void*>				LeftMouseButtonReleasedFlowOut();
	rxcpp::observable<void*>				RightMouseButtonDownFlowOut();
	rxcpp::observable<void*>				RightMouseButtonReleasedFlowOut();
	rxcpp::observable<mouse_pos_t>			LeftMouseButtonPosFlowOut();
	rxcpp::observable<mouse_pos_t>			RightMouseButtonPosFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class ViewportView
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_VIEWS_VIEWPORTVIEW_H__