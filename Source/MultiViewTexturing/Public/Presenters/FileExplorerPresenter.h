#ifndef	__FUSION_PUBLIC_PRESENTERS_FILEEXPLORERPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_FILEEXPLORERPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>


namespace fu {
namespace app {
class WidgetRepo;
}	///	!namespace app
namespace mvt {
///
class FileExplorerModel;
class FileExplorerView;
///	\class FileExploererPresenter
///	\brief
class FileExplorerPresenter : public app::Initializable
{
public:
	using model_ptr_t = std::shared_ptr<FileExplorerModel>;
	using view_ptr_t = std::shared_ptr<FileExplorerView>;
	using wrepo_ptr_t = std::shared_ptr<app::WidgetRepo>;
	/// Construction
	FileExplorerPresenter(model_ptr_t model, view_ptr_t view, wrepo_ptr_t wrepo);
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FileExplorerPresenter
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_FILEEXPLORERPRESENTER_H__