#ifndef	__FUSION_PUBLIC_VIEWS_NORMALSRESULTVIEW_H__
#define __FUSION_PUBLIC_VIEWS_NORMALSRESULTVIEW_H__

#include <Buffer.h>
#include <Widget.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {

class NormalsResultView : public app::Widget
{
public:
	NormalsResultView();

	void Render() override;

	rxcpp::observer<BufferCPU<uchar4>> NormalsImageFlowIn();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class NormalsResultView
}	///	!namesapce fusion
}	///	!namesapce fu
#endif	///	!__FUSION_PUBLIC_VIEWS_NORMALSRESULTVIEW_H__