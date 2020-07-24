#ifndef	__FUSION_PUBLIC_PRESENTERS_OMNICONNECTMENUPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_OMNICONNECTMENUPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;
}
namespace fusion {

class MainToolbarView;
class OmniconnectModel;
class ProjectModel;
class Coordination;
class OmniconnectVideoListView;
class PlayerModel;
class ProgressBarView;
class FusedExportModel;

class OmniconnectMenuPresenter : public app::Initializable
{
public:
	using toolbar_view_ptr_t	= std::shared_ptr<MainToolbarView>;
	using model_ptr_t			= std::shared_ptr<OmniconnectModel>;
	using prj_model_ptr_t		= std::shared_ptr<ProjectModel>;
	using coord_ptr_t			= std::shared_ptr<Coordination>;
	using wrepo_ptr_t			= std::shared_ptr<app::WidgetRepo>;
	using vlist_view_ptr_t		= std::shared_ptr<OmniconnectVideoListView>;
	using player_model_ptr_t	= std::shared_ptr<PlayerModel>;
	using prog_view_ptr_t		= std::shared_ptr<ProgressBarView>;
	using export_model_ptr_t	= std::shared_ptr<FusedExportModel>;

	OmniconnectMenuPresenter(model_ptr_t model, toolbar_view_ptr_t toolbar_view, prj_model_ptr_t prj_model, coord_ptr_t coord, wrepo_ptr_t wrepo, vlist_view_ptr_t vlist_view, player_model_ptr_t player_model, prog_view_ptr_t prog_view, export_model_ptr_t export_model);
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FileMenuPresenter
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_OMNICONNECTMENUPRESENTER_H__