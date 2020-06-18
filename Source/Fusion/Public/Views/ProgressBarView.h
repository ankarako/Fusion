#ifndef __FUSION_PUBLIC_VIEWS_PROGRESSBARVIEW_H__
#define __FUSION_PUBLIC_VIEWS_PROGRESSBARVIEW_H__

#include <Widget.h>
#include <spimpl.h>
#include <array>
#include <string>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {

class ProgressBarView : public app::Widget
{
public:
	using pos_t = std::array<float, 2>;
	ProgressBarView();
	void Render() override;

	rxcpp::observer<pos_t> PositionFlowIn();
	rxcpp::observer<float> ProgressFlowIn();
	rxcpp::observer<std::string> MessageFlowIn();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class ProgressBarView
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_VIEWS_PROGRESSBARVIEW_H__