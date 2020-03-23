#ifndef	__FUSION_PUBLIC_MODELS_ILLUMINATIONESTIMATIONMODEL_H__
#define __FUSION_PUBLIC_MODELS_ILLUMINATIONESTIMATIONMODEL_H__

#include <Initializable.h>
#include <Buffer.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {

class ProjectModel;


class IlluminationEstimationModel : public app::Initializable
{
public:
	using prj_model_ptr_t = std::shared_ptr<ProjectModel>;

	IlluminationEstimationModel(prj_model_ptr_t prj_model);
	///
	void Init() override;
	rxcpp::observer<std::pair<uint2, BufferCPU<uchar4>>> 	FrameFlowIn();
	rxcpp::observable<BufferCPU<uchar4>>					IlluminationMapFlowOut(); 
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class IlluminationEstimationModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_ILLUMINATIONESTIMATIONMODEL_H__