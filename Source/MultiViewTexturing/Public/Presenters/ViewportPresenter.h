#ifndef	__MVT_PUBLIC_PRESENTERS_VIEWPORTPRESENTER_H__
#define __MVT_PUBLIC_PRESENTERS_VIEWPORTPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;
}
namespace mvt {
class ViewportView;
class ViewportTracingModel;
class Coordination;
///	\class ViewportPresenter
///	\brief Viewport-Models interaction
class ViewportPresenter : public app::Initializable
{
public:
	///	\typedef view_ptr_t
	///	\brief a shared pointer to a viewport view
	using view_ptr_t = std::shared_ptr<ViewportView>;
	///	\typedef model_ptr_t
	///	\brief a shared pointer to a viewport tracing model
	using model_ptr_t = std::shared_ptr<ViewportTracingModel>;
	///	\typedef wrepo_ptr_t
	///	\brief	a shared pointer to a widget repo
	using wrepo_ptr_t = std::shared_ptr<app::WidgetRepo>;
	///	\typedef coord_ptr_t
	///	\brief a shared pointer to a coordination object
	using coord_ptr_t = std::shared_ptr<Coordination>;
	/// Construction
	///	\brief dependency injection
	ViewportPresenter(view_ptr_t view, model_ptr_t model, wrepo_ptr_t wrepo, coord_ptr_t coord);
	///	\brief presenter initialization
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class ViewportPresenter
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_PRESENTERS_VIEWPORTPRESENTER_H__