#ifndef __MVT_PUBLIC_VIEWS_MAINTOOLBARVIEW_H__
#define __MVT_PUBLIC_VIEWS_MAINTOOLBARVIEW_H__

#include <Widget.h>
#include <spimpl.h>

namespace fu {
namespace app {
class FontManager;
}
namespace mvt {
///	\class MainToolbarView
///	\brief MainToolbar UI interaction
class MainToolbarView : public app::Widget
{
public:
	///	\typedef fman_ptr_t 
	///	\brief a shared pointer to the font manager
	using fman_ptr_t = std::shared_ptr<app::FontManager>;
	/// Construction
	///	\brief default constructor
	MainToolbarView(fman_ptr_t fman);
	///	\brief render the toolbar
	void Render() override;
	/// outputs
	/// \brief subscribe to an "import performance" event
	rxcpp::observable<void*> OnFileMenu_ImportPerformanceClicked();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class MainToolbarView
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_VIEWS_MAINTOOLBARVIEW_H__