#ifndef	__FUSION_PUBLIC_VIEWS_RAYTRACINGCONTROLVIEW_H__
#define __FUSION_PUBLIC_VIEWS_RAYTRACINGCONTROLVIEW_H__

#include <Widget.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {

class RayTracingControlView : public app::Widget
{
public:
	RayTracingControlView();
	void Render() override;
	rxcpp::observable<float> OnCullingPlanePositionChanged();
	rxcpp::observable<float> OnPclSizeChanged();
	rxcpp::observable<bool>	OnRenderNormalsFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class RayTracingControlView
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_VIEWS_RAYTRACINGCONTROLVIEW_H__