#ifndef	__FUSION_PUBLIC_PRESENTERS_PLAYERCONTROLLERPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_PLAYERCONTROLLERPRESENTER_H__

#include <Initializable.h>
#include <Destroyable.h>
#include <memory>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;
}	///	!namespace app
namespace fusion {
///
class PlayerControllerView;
class PlayerModel;
class FileExplorerView;
class ProgressBarView;
class Coordination;
///	\class PlayerControllerPresenter
///	\brief presenter for controlling playeback
class PlayerControllerPresenter : public app::Initializable, public app::Destroyable
{
public:
	using coord_ptr_t = std::shared_ptr<Coordination>;
	///	\typedef
	using model_ptr_t = std::shared_ptr<PlayerModel>;
	///	\typedef
	using view_ptr_t = std::shared_ptr<PlayerControllerView>;
	///	\typedef
	using wrepo_ptr_t = std::shared_ptr<app::WidgetRepo>;
	///	\typedef 
	using fexp_view_ptr_t = std::shared_ptr<FileExplorerView>;
	using prog_view_ptr_t = std::shared_ptr<ProgressBarView>;
	/// Construction
	///	\brief construct from model and view
	PlayerControllerPresenter(
		model_ptr_t model, 
		view_ptr_t view, 
		fexp_view_ptr_t fexp_view, 
		prog_view_ptr_t prog_view,
		wrepo_ptr_t wrepo,
		coord_ptr_t coord);
	///	\brief initialize the presenter
	///	makes aubscriptions
	void Init() override;
	///	\brief destroy the presenter
	///	destroys the decoding context
	void Destroy() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};
}	///	!namespace fu
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_PRESENTERS_PLAYERCONTROLLERPRESENTER_H__