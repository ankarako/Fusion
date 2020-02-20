#ifndef	__FUSION_PUBLIC_PRESENTERS_PLAYERVIEWPORTPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_PLAYERVIEWPORTPRESENTER_H__

#include <Initializable.h>

#include <memory>
#include <spimpl.h>

namespace app {
class WidgetRepo;
}

namespace fusion {
class PlayerModel;
class PlayerViewportView;

class PlayerViewportPresenter : public app::Initializable
{
public:
	using model_ptr_t = std::shared_ptr<PlayerModel>;
	using view_ptr_t = std::shared_ptr<PlayerViewportView>;
	using wrepo_ptr_t = std::shared_ptr<app::WidgetRepo>;
	
	PlayerViewportPresenter(model_ptr_t model, view_ptr_t view, wrepo_ptr_t wrepo);
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PlayerViewportPresenter
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_PRESENTERS_PLAYERVIEWPORTPRESENTER_H__