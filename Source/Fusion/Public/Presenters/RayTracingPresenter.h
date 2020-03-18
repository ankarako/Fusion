#ifndef	__FUSION_PUBLIC_PRESENTERS_RAYTRACINGPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_RAYTRACINGPRESENTER_H__

#include <Initializable.h>
#include <WidgetRepo.h>
#include <memory>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;
}
namespace rt {
class AssetLoadingSystem;
}
namespace fusion {
///	
class FileExplorerView;
class RayTracingModel;
class RayTracingView;
class Coordination;
class SettingsRepo;
class DepthEstimationModel;
class RayTracingControlView;
///	\class RayTracingPresenter
///	\brief Ray tracing model's presenter
class RayTracingPresenter : public app::Initializable
{
public:
	using fexp_view_ptr_t 	= std::shared_ptr<FileExplorerView>;
	using model_ptr_t		= std::shared_ptr<RayTracingModel>;
	using view_ptr_t		= std::shared_ptr<RayTracingView>;
	using wrepo_ptr_t		= std::shared_ptr<app::WidgetRepo>;
	using coord_ptr_t		= std::shared_ptr<Coordination>;
	using asset_syst_ptr_t	= std::shared_ptr<rt::AssetLoadingSystem>;
	using srepo_ptr_t		= std::shared_ptr<SettingsRepo>;
	using dest_model_ptr_t	= std::shared_ptr<DepthEstimationModel>;
	using rt_ctrl_view_ptr_t = std::shared_ptr<RayTracingControlView>;
	/// Construction
	RayTracingPresenter(
		fexp_view_ptr_t fexpView, 
		model_ptr_t model, 
		view_ptr_t view, 
		wrepo_ptr_t wrepo, 
		coord_ptr_t coord, 
		asset_syst_ptr_t asset_syst, 
		srepo_ptr_t srepo, 
		dest_model_ptr_t dest_model,
		rt_ctrl_view_ptr_t rt_ctrl_view);
	///
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_RAYTRACINGPRESENTER_H__