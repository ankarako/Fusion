#ifndef __FUSION_PUBLIC_MODELS_DEPTHESTIMATIONMODEL_H__
#define __FUSION_PUBLIC_MODELS_DEPTHESTIMATIONMODEL_H__

#include <Initializable.h>
#include <Buffer.h>
#include <spimpl.h>
#include <string>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {
class ProjectModel;
class Coordination;
class SettingsRepo;

class DepthEstimationModel : public app::Initializable
{
public:
	using prj_model_ptr_t	= std::shared_ptr<ProjectModel>;
	using coord_ptr_t		= std::shared_ptr<Coordination>;
	using srepo_ptr_t		= std::shared_ptr<SettingsRepo>;

	DepthEstimationModel(prj_model_ptr_t prj_model, coord_ptr_t coord, srepo_ptr_t srepo);
	///
	void Init() override;
	rxcpp::observer<std::pair<uint2, BufferCPU<uchar4>>> 	FrameFlowIn();
	rxcpp::observable<std::string>							PointCloudFilepathFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class DepthEstimationModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_DEPTHESTIMATIONMODEL_H__