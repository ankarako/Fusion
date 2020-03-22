#ifndef	__FUSION_PUBLIC_PRESENTERS_MAINTOOLBARPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_MAINTOOLBARPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;
}
namespace fusion {
///
class FileMenuPresenter;
class FiltersMenuPresenter;
class RayTracingControlView;
class PerfcapPlayerModel;
///	\class MainToolbarPresenter
///	\brief responsible for interacting with the main toolbar
class MainToolbarPresenter	: public app::Initializable
{
public:
	using file_menu_ptr_t = std::shared_ptr<FileMenuPresenter>;
	using filters_menu_ptr_t = std::shared_ptr<FiltersMenuPresenter>;
	using rt_ctrl_view_ptr_t	= std::shared_ptr<RayTracingControlView>;
	using perfcap_model_ptr_t	= std::shared_ptr<PerfcapPlayerModel>;
	/// Construction
	MainToolbarPresenter(
		file_menu_ptr_t		file_menu,
		filters_menu_ptr_t	filters_menu);
	/// \brief Initialization
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class MainTollbarPresenter
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_MAINTOOLBARPRESENTER_H__