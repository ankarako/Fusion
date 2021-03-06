#ifndef __FUSION_PUBLIC_VIEWS_RAYTRACINGVIEW_H__
#define __FUSION_PUBLIC_VIEWS_RAYTRACINGVIEW_H__

#include <Initializable.h>
#include <Widget.h>
#include <Buffer.h>
#include <rxcpp/rx.hpp>
#include <array>
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
	using mat_t = std::array<float, 16>;
	using trans_vec_t = std::array<float, 3>;
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
	///
	rxcpp::observable<mat_t> RotationTransformFlowOut();
	rxcpp::observable<trans_vec_t>	TranslationFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class RayTracingView
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_VIEWS_RAYTRACINGVIEW_H__