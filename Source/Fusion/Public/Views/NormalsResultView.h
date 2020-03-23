#ifndef	__FUSION_PUBLIC_VIEWS_NORMALSRESULTVIEW_H__
#define __FUSION_PUBLIC_VIEWS_NORMALSRESULTVIEW_H__

#include <Buffer.h>
#include <Widget.h>
#include <Initializable.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {

class Coordination;

class NormalsResultView : public app::Widget, public app::Initializable
{
public:
	using coord_ptr_t = std::shared_ptr<Coordination>;

	NormalsResultView(coord_ptr_t coord);

	void Init() override;
	void Render() override;

	rxcpp::observer<BufferCPU<uchar4>> NormalsImageFlowIn();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class NormalsResultView
}	///	!namesapce fusion
}	///	!namesapce fu
#endif	///	!__FUSION_PUBLIC_VIEWS_NORMALSRESULTVIEW_H__