#ifndef	__MVT_PUBLIC_PRESENTERS_FILEMENUPRESENTER_H__
#define __MVT_PUBLIC_PRESENTERS_FILEMENUPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;
}
namespace mvt {
class MainToolbarView;
class PerformanceImportModel;
class FileExplorerView;
class Coordination;
///	\class MainToolbarPresenter
///	\brief interaction with the main toolbar and the application's models
class FileMenuPresenter : public app::Initializable
{
public:
	/// \typedef view_ptr_t
	///	\brief a shared pointer to a MainToolbarView
	using view_ptr_t = std::shared_ptr<MainToolbarView>;
	///	\typedef wrepo_ptr_t
	///	\brief a shared pointer to a WidgetRepo
	using wrepo_ptr_t = std::shared_ptr<app::WidgetRepo>;
	///	\typedef perf_import_model_ptr_t
	///	\brief a shared pointer to a performance import model
	using perf_import_model_ptr_t = std::shared_ptr<PerformanceImportModel>;
	///	\typedef fexp_view_ptr_t 
	///	\brief a shared pointer to a file explorer view
	using fexp_view_ptr_t = std::shared_ptr<FileExplorerView>;
	///	\typedef coord_ptr_t
	///	\brief a shared pointer to a coordination object
	using coord_ptr_t = std::shared_ptr<Coordination>;
	/// Construction
	///	\brief depenencies
	FileMenuPresenter(view_ptr_t view, wrepo_ptr_t wrepo, perf_import_model_ptr_t perf_import_model, fexp_view_ptr_t fexp_view, coord_ptr_t coord);
	///	\brief Initialize the presenter
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FileMenuPresenter
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_PRESENTERS_FILEMENUPRESENTER_H__
