#ifndef	__FUSION_PUBLIC_VIEWS_MAINTOOLBARVIEW_H__
#define __FUSION_PUBLIC_VIEWS_MAINTOOLBARVIEW_H__

#include <Widget.h>
#include <spimpl.h>

namespace fu {
namespace app {
class FontManager;
}
namespace fusion {
///	\class MAinToolbarView
///	\brief view for the main toolbar
class MainToolbarView	: public app::Widget
{
public:
	using fman_ptr_t = std::shared_ptr<app::FontManager>;
	/// Construction
	MainToolbarView(fman_ptr_t fman);
	/// \brief Widget rendering
	void Render() override;
	///	filemenu observables
	rxcpp::observable<void*> OnFileMenu_OpenProjectClicked();
	rxcpp::observable<void*> OnFileMenu_SaveProjectAsClicked();
	rxcpp::observable<void*> OnFileMenu_SaveProjectClicked();
	rxcpp::observable<void*> OnFileMenu_OpenVideoFileClicked();
	rxcpp::observable<void*> OnFileMenu_Open3DFileClicked();
	rxcpp::observable<void*> OnFiltersMenu_EstimateDepthClicked();
	rxcpp::observable<void*> OnWindowsMenu_RayTracingControlClicked();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class MainToolbarView
}	///	!namespace fusion
}
#endif	///	!__FUSION_PUBLIC_VIEWS_MAINTOOLBARVIEW_H__