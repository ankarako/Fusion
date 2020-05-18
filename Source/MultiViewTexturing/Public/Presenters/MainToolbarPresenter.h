#ifndef	__MVT_PUBLIC_PRESENTERS_MAINTOOLBARPRESENTER_H__
#define __MVT_PUBLIC_PRESENTERS_MAINTOOLBARPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;	
}
namespace mvt {
class MainToolbarView;
///	\class MainToolbarPresenter
///	\brief interaction with the main toolbar and the application's models
class MainToolbarPresenter : public app::Initializable
{
public:
	/// \typedef view_ptr_t
	///	\brief a shared pointer to a MainToolbarView
	using view_ptr_t = std::shared_ptr<MainToolbarView>;
	///	\typedef wrepo_ptr_t
	///	\brief a shared pointer to a WidgetRepo
	using wrepo_ptr_t = std::shared_ptr<app::WidgetRepo>;
	/// Construction
	///	\brief depenencies
	MainToolbarPresenter(view_ptr_t view, wrepo_ptr_t wrepo);
	///	\brief Initialize the presenter
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class MainToolbarPresenter
}
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_PRESENTERS_MAINTOOLBARPRESENTER_H__
