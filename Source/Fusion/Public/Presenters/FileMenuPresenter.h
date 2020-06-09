#ifndef	__FUSION_PUBLIC_PRESENTERS_FILEMENUPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_FILEMENUPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;
}
namespace fusion {

class MainToolbarView;
class FileExplorerView;
class PlayerModel;
class ProjectModel;
class Coordination;
class PerfcapPlayerModel;
class SettingsRepo;

class FileMenuPresenter	: public app::Initializable
{
public:
	using wrepo_ptr_t = std::shared_ptr<app::WidgetRepo>;
	using player_model_ptr_t = std::shared_ptr<PlayerModel>;
	using view_ptr_t = std::shared_ptr<MainToolbarView>;
	using fexp_view_ptr_t = std::shared_ptr<FileExplorerView>;
	using prj_model_ptr_t = std::shared_ptr<ProjectModel>;
	using coord_ptr_t = std::shared_ptr<Coordination>;
	using perfcap_model_ptr_t = std::shared_ptr<PerfcapPlayerModel>;
	using srepo_ptr_t = std::shared_ptr<SettingsRepo>;
	/// Construction
	FileMenuPresenter(
		player_model_ptr_t	decoder_model,
		view_ptr_t			view,
		fexp_view_ptr_t		fexp_view,
		wrepo_ptr_t			wrepo,
		prj_model_ptr_t		prj_model,
		coord_ptr_t			coord,
		perfcap_model_ptr_t perfcap_model,
		srepo_ptr_t			srepo);

	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FileMenuPresenter
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_FILEMENUPRESENTER_H__