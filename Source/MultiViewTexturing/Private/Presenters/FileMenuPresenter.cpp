#include <Presenters/FileMenuPresenter.h>
#include <Views/MainToolbarView.h>
#include <Views/FileExplorerView.h>
#include <Models/PerformanceImportModel.h>
#include <WidgetRepo.h>
#include <Core/Coordination.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief MainToolbarPresenter Implementation
struct FileMenuPresenter::Impl
{
	view_ptr_t 				m_View;
	wrepo_ptr_t 			m_Wrepo;
	perf_import_model_ptr_t m_PerformanceImportModel;
	fexp_view_ptr_t			m_FexpView;
	coord_ptr_t				m_Coord;
	/// Construction
	Impl(view_ptr_t view, wrepo_ptr_t wrepo, perf_import_model_ptr_t perf_import_model, fexp_view_ptr_t fexp_view, coord_ptr_t coord)
		: m_View(view)
		, m_Wrepo(wrepo)
		, m_PerformanceImportModel(perf_import_model)
		, m_FexpView(fexp_view)
		, m_Coord(coord)
	{ }
};	///	!struct Impl
/// Construction
FileMenuPresenter::FileMenuPresenter(view_ptr_t view, wrepo_ptr_t wrepo, perf_import_model_ptr_t perf_import_model, fexp_view_ptr_t fexp_view, coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, wrepo, perf_import_model, fexp_view, coord))
{ }
///	\brief Initialize the presenter
void FileMenuPresenter::Init()
{
	///============================
	/// Import Performance
	/// MainToolbar -> FileExplorer
	///=============================
	m_Impl->m_View->OnFileMenu_ImportPerformanceClicked()
		.subscribe([this](auto _)
	{
		m_Impl->m_FexpView->SetMode(FileExplorerMode::ImportPerformance);
		m_Impl->m_FexpView->Activate();
	});
	///==========================================
	/// Import Perfoemance
	/// FileExplorer -> ImportPerformance Moddel
	///===========================================
	m_Impl->m_FexpView->ImportPerformanceFlowOut()
		.observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe(m_Impl->m_PerformanceImportModel->ImportFilepathFlowIn());
}
}	///	!namespace mvt
}	///	!namespace fu