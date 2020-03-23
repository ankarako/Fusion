#ifndef	__FUSION_PUBLIC_PRESENTERS_FILTERSMENUPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_FILTERSMENUPRESENTER_H__

#include <Buffer.h>
#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;
}
namespace fusion {
class MainToolbarView;
class PlayerModel;
class ProjectModel;
class DepthEstimationSettingsView;
class DepthEstimationModel;
class NormalsEstimationModel;
class IlluminationEstimationModel;
class Coordination;

class FiltersMenuPresenter : public app::Initializable
{
public:
	using view_ptr_t				= std::shared_ptr<MainToolbarView>;
	using player_model_ptr_t		= std::shared_ptr<PlayerModel>;
	using prj_model_ptr_t			= std::shared_ptr<ProjectModel>;
	using depth_est_model_ptr_t		= std::shared_ptr<DepthEstimationModel>;
	using depth_est_view_ptr_t		= std::shared_ptr<DepthEstimationSettingsView>;
	using normal_est_model_ptr_t	= std::shared_ptr<NormalsEstimationModel>;
	using illum_model_ptr_t			= std::shared_ptr<IlluminationEstimationModel>;
	using wrepo_ptr_t				= std::shared_ptr<app::WidgetRepo>;
	using coord_ptr_t				= std::shared_ptr<Coordination>;

	FiltersMenuPresenter(
		view_ptr_t				view,
		player_model_ptr_t		player_model,
		prj_model_ptr_t			prj_model,
		depth_est_model_ptr_t	depth_est_model,
		depth_est_view_ptr_t	depth_est_view,
		normal_est_model_ptr_t	normal_est_model,
		illum_model_ptr_t		illum_model,
		wrepo_ptr_t				wrepo,
		coord_ptr_t				coord);

	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FileMenuPresenter
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_FILTERSMENUPRESENTER_H__