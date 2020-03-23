#ifndef	__FUSION_PUBLIC_PRESENTERS_PLAYERVIEWPORTPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_PLAYERVIEWPORTPRESENTER_H__

#include <Initializable.h>

#include <memory>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;
}
namespace fusion {

class PlayerViewportView;
class PlayerModel;
class VideoTracingModel;
class Coordination;
class FileExplorerView;
class AssetLoadingModel;
class PerfcapPlayerModel;
///	\class PlayerViewportPresenter
///	\brief presents the video frames
class PlayerViewportPresenter : public app::Initializable
{
public:
	using model_ptr_t			= std::shared_ptr<PlayerModel>;
	using view_ptr_t			= std::shared_ptr<PlayerViewportView>;
	using wrepo_ptr_t			= std::shared_ptr<app::WidgetRepo>;
	using tracer_model_ptr_t	= std::shared_ptr<VideoTracingModel>;
	using coord_ptr_t			= std::shared_ptr<Coordination>;
	using fexp_view_ptr_t		= std::shared_ptr<FileExplorerView>;
	using asset_model_ptr_t		= std::shared_ptr<AssetLoadingModel>;
	using perfcap_model_ptr_t	= std::shared_ptr<PerfcapPlayerModel>;
	/// Construction
	PlayerViewportPresenter(
		model_ptr_t model, 
		view_ptr_t view, 
		tracer_model_ptr_t tracer_model, 
		wrepo_ptr_t wrepo, 
		coord_ptr_t coord,
		fexp_view_ptr_t fexp_view,
		asset_model_ptr_t asset_model,
		perfcap_model_ptr_t perfcap_model);
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PlayerViewportPresenter
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_PLAYERVIEWPORTPRESENTER_H__