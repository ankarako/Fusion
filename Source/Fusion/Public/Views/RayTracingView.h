#ifndef __FUSION_PUBLIC_VIEWS_RAYTRACINGVIEW_H__
#define __FUSION_PUBLIC_VIEWS_RAYTRACINGVIEW_H__

#include <Initializable.h>
#include <Widget.h>
#include <Buffer.h>
#include <rxcpp/rx.hpp>
#include <spimpl.h>

namespace fu {
namespace app {
class FontManager;
}
namespace fusion {
class Coordination;
///	\class RayTracingView
///	\brief our 3D viewport
class RayTracingView : public app::Initializable, public app::Widget
{
public:
	using fman_ptr_t	= std::shared_ptr<app::FontManager>;
	using coord_ptr_t	= std::shared_ptr<Coordination>;
	/// Construction
	RayTracingView(fman_ptr_t fman, coord_ptr_t coord);
	///
	void Init() override;
	///
	void Render() override;
	/// inputs
	rxcpp::observer<BufferCPU<uchar4>>	FrameFlowIn();
	/// output events
	rxcpp::observable<float2>	OnViewportSizeChanged();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class RayTracingView
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_VIEWS_RAYTRACINGVIEW_H__