#ifndef __FUSION_PUBLIC_MODELS_OMNICONNECTMODEL_H__
#define __FUSION_PUBLIC_MODELS_OMNICONNECTMODEL_H__

#include <Initializable.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>
#include <string>
#include <Core/OmniconnectVideoElement.h>

namespace fu {
namespace fusion {

class ProjectSettings;
class SettingsRepo;

class OmniconnectModel : public app::Initializable
{
public:
	using prj_set_ptr_t = std::shared_ptr<ProjectSettings>;
	using srepo_ptr_t	= std::shared_ptr<SettingsRepo>;

	OmniconnectModel(srepo_ptr_t srepo);
	void Init() override;

	rxcpp::observer<prj_set_ptr_t>	ProjectSettingsFlowIn();
	rxcpp::observer<void*>			ShowMediaListFlowIn();
	rxcpp::observer<int>			VideoListIndexFlowIn();
	rxcpp::observable<std::string>	VideoFilepathFlowOut();
	rxcpp::observable<omni_video_list_ptr_t>	VideoListFlowOut();
	rxcpp::observable<float>					ProgressFlowOut();
	rxcpp::observer<void*>						UploadMentorLayerFlowIn();
	rxcpp::observer<void*>						ExportMentorFileFlowIn();
	rxcpp::observable<std::string>				ExportMentorLayerFilepathFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class OmniconnectModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_OMNICONNECTMODEL_H__