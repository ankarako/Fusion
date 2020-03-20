#ifndef	__FUSION_PUBLIC_PRESENTERS_MAINTOOLBARPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_MAINTOOLBARPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;
}
namespace fusion {
///
class MainToolbarView;
class FileExplorerView;
class PlayerModel;
class ProjectModel;
class DepthEstimationSettingsView;
class DepthEstimationModel;
class Coordination;
class RayTracingControlView;
class PerfcapPlayerModel;
///	\class MainToolbarPresenter
///	\brief responsible for interacting with the main toolbar
class MainToolbarPresenter	: public app::Initializable
{
public:
	using wrepo_ptr_t = std::shared_ptr<app::WidgetRepo>;
	using player_model_ptr_t = std::shared_ptr<PlayerModel>;
	using view_ptr_t = std::shared_ptr<MainToolbarView>;
	using fexp_view_ptr_t = std::shared_ptr<FileExplorerView>;
	using prj_model_ptr_t = std::shared_ptr<ProjectModel>;
	using dest_set_view_ptr_t = std::shared_ptr<DepthEstimationSettingsView>;
	using dest_model_ptr_t = std::shared_ptr<DepthEstimationModel>;
	using coord_ptr_t = std::shared_ptr<Coordination>;
	using rt_ctrl_view_ptr_t = std::shared_ptr<RayTracingControlView>;
	using perfcap_model_ptr_t = std::shared_ptr<PerfcapPlayerModel>;
	/// Construction
	MainToolbarPresenter(
		player_model_ptr_t decoder_model, 
		view_ptr_t view, 
		fexp_view_ptr_t fexp_view, 
		wrepo_ptr_t wrepo, 
		prj_model_ptr_t prj_model, 
		dest_set_view_ptr_t dest_view, 
		dest_model_ptr_t dest_model, 
		coord_ptr_t coord,
		rt_ctrl_view_ptr_t rt_ctrl_view,
		perfcap_model_ptr_t perfcap_model);
	/// \brief Initialization
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class MainTollbarPresenter
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_MAINTOOLBARPRESENTER_H__