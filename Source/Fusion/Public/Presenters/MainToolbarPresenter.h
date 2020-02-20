#ifndef	__FUSION_PUBLIC_PRESENTERS_MAINTOOLBARPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_MAINTOOLBARPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace app {
class WidgetRepo;	
}

namespace fusion {
///
class PlayerModel;
///
class MainToolbarView;
///
class FileExplorerView;
///	\class MainToolbarPresenter
///	\brief responsible for interacting with the main toolbar
class MainToolbarPresenter	: public app::Initializable
{
public:
	using wrepo_ptr_t = std::shared_ptr<app::WidgetRepo>;
	using player_model_ptr_t = std::shared_ptr<PlayerModel>;
	using view_ptr_t = std::shared_ptr<MainToolbarView>;
	using fexp_view_ptr_t = std::shared_ptr<FileExplorerView>;
	/// Construction
	MainToolbarPresenter(player_model_ptr_t player_model, view_ptr_t view, fexp_view_ptr_t fexp_view, wrepo_ptr_t wrepo);
	/// \brief Initialization
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class MainTollbarPresenter
}	///	!namespace 
#endif	///	!__FUSION_PUBLIC_PRESENTERS_MAINTOOLBARPRESENTER_H__