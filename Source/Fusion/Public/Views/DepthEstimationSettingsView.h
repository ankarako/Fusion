#ifndef	__FUSION_PUBLIC_VIEWS_DEPTHESTIMATIONSETTINGSVIEW_H__
#define __FUSION_PUBLIC_VIEWS_DEPTHESTIMATIONSETTINGSVIEW_H__

#include <Widget.h>
#include <Core/DepthEstimationModels.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>
#include <string>

namespace fu {
namespace fusion {

class DepthEstimationSettingsView : public app::Widget
{
public:
	/// Construction
	DepthEstimationSettingsView();
	///
	void Render() override;

	rxcpp::observable<DepthEstimationModels>		OnDepthEstimationModelChanged();
	rxcpp::observable<void*>					OnEstimateDepthButtonClicked();
	rxcpp::observable<void*>					OnCancelButtonClicked();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class DepthEstimationSettingsView
}	///	!namesapce fusion
}	/// !namespace fu
#endif	///	!__FUSION_PUBLIC_VIEWS_DEPTHESTIMATIONSETTINGSVIEW_H__