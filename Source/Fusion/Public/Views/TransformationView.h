#ifndef __FUSION_PUBLIC_VIEWS_TRANSFORMATIONVIEW_H__
#define __FUSION_PUBLIC_VIEWS_TRANSFORMATIONVIEW_H__

#include <Widget.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>
#include <array>

namespace fu {
namespace fusion {

class TransformationView : public app::Widget
{
public:
	using trans_vec_t	= std::array<float, 3>;
	using trans_t		= std::array<float, 16>;
	/// Construction
	TransformationView();
	void Render() override;
	rxcpp::observable<trans_vec_t>	TranslationVectorFlowOut();	
	rxcpp::observable<trans_vec_t>	RotationVectorFlowOut();
	rxcpp::observable<float>		UniformScaleFlowOut();
	rxcpp::observable<trans_t>		TransformFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class TranslationView
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_VIEWS_TRANSFORMATIONVIEW_H__