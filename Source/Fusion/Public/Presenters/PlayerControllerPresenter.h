#ifndef	__FUSION_PUBLIC_PRESENTERS_PLAYERCONTROLLERPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_PLAYERCONTROLLERPRESENTER_H__

#include <Initializable.h>

#include <memory>
#include <spimpl.h>

namespace app {
class WidgetRepo;
}	///	!namespace app
namespace fusion {
///
class PlayerModel;
class PlayerControllerView;

///	\class PlayerControllerPresenter
///	\brief presenter for controlling playeback
class PlayerControllerPresenter : public app::Initializable
{
public:
	///	\typedef
	using model_ptr_t = std::shared_ptr<PlayerModel>;
	///	\typedef
	using view_ptr_t = std::shared_ptr<PlayerControllerView>;
	///	\typedef
	using wrepo_ptr_t = std::shared_ptr<app::WidgetRepo>;
	/// Construction
	///	\brief construct from model and view
	PlayerControllerPresenter(model_ptr_t model, view_ptr_t view, wrepo_ptr_t wrepo);
	///	\brief initialize the presenter
	///	makes aubscriptions
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_PRESENTERS_PLAYERCONTROLLERPRESENTER_H__