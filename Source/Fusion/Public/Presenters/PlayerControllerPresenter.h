#ifndef	__FUSION_PUBLIC_PRESENTERS_PLAYERCONTROLLERPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_PLAYERCONTROLLERPRESENTER_H__

#include <Initializable.h>
#include <Destroyable.h>
#include <memory>
#include <spimpl.h>
#include <taskflow/taskflow.hpp>

namespace app {
class WidgetRepo;
}	///	!namespace app
namespace fu {
namespace trans {
///	\class DecodingContext
class DecodingContext;
}	///	!namespace trans
namespace fusion {
///
class PlayerControllerView;
class FileExplorerView;

///	\class PlayerControllerPresenter
///	\brief presenter for controlling playeback
class PlayerControllerPresenter : public app::Initializable, public app::Destroyable
{
public:
	///	\typedef executor_ptr_t
	using executor_ptr_t = std::shared_ptr<tf::Executor>;
	///	\typedef
	using model_ptr_t = std::shared_ptr<trans::DecodingContext>;
	///	\typedef
	using view_ptr_t = std::shared_ptr<PlayerControllerView>;
	///	\typedef
	using wrepo_ptr_t = std::shared_ptr<app::WidgetRepo>;
	///	\typedef 
	using fexp_view_ptr_t = std::shared_ptr<FileExplorerView>;
	/// Construction
	///	\brief construct from model and view
	PlayerControllerPresenter(
		model_ptr_t model, 
		view_ptr_t view, 
		fexp_view_ptr_t fexp_view, 
		wrepo_ptr_t wrepo, 
		executor_ptr_t exec);
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