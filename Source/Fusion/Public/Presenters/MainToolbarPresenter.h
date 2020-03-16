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
	/// Construction
	MainToolbarPresenter(player_model_ptr_t decoder_model, view_ptr_t view, fexp_view_ptr_t fexp_view, wrepo_ptr_t wrepo, prj_model_ptr_t prj_model);
	/// \brief Initialization
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class MainTollbarPresenter
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_MAINTOOLBARPRESENTER_H__