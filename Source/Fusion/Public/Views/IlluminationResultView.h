#ifndef	__FUSION_PUBLIC_VIEWS_ILLUMINATIONRESULTVIEW_H__
#define __FUSION_PUBLIC_VIEWS_ILLUMINATIONRESULTVIEW_H__

#include <Initializable.h>
#include <Widget.h>
#include <Buffer.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {

class IlluminationResultView : public app::Widget, public app::Initializable
{
public:
	IlluminationResultView();

	void Init() override;
	void Render() override;

	rxcpp::observer<BufferCPU<uchar4>> IlluminationMapFlowIn();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};
}	///	!namespace fusion 
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_VIEWS_ILLUMINATIONRESULTVIEW_H__