#ifndef	__FUSION_PUBLIC_PRESENTERS_WINDOWSMENUPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_WINDOWSMENUPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;
}
namespace fusion {

class MainToolbarView;
class RayTracingControlView;
class NormalsResultView;
class IlluminationResultView;
class WindowsMenuModel;

class WindowsMenuPresenter : public app::Initializable
{
public:
	using view_ptr_t			= std::shared_ptr<MainToolbarView>;
	using rt_ctrl_view_ptr		= std::shared_ptr<RayTracingControlView>;
	using norm_res_view_ptr_t	= std::shared_ptr<NormalsResultView>;
	using illum_view_ptr_t		= std::shared_ptr<IlluminationResultView>;
	using model_ptr_t			= std::shared_ptr<WindowsMenuModel>;
	using wrepo_ptr_t			= std::shared_ptr<app::WidgetRepo>;

	WindowsMenuPresenter(
		view_ptr_t view, 
		rt_ctrl_view_ptr rt_ctrl_view, 
		norm_res_view_ptr_t norm_res_view, 
		illum_view_ptr_t illum_view, 
		model_ptr_t model,
		wrepo_ptr_t wrepo);
	void Init();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FileMenuPresenter
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_WINDOWSMENUPRESENTER_H__