#include <Presenters/FuImportPresenter.h>
#include <Models/FuImportModel.h>
#include <Models/ProjectModel.h>
#include <Views/FileExplorerView.h>

namespace fu {
namespace fusion {

struct FuImportPresenter::Impl
{
	model_ptr_t		m_Model;
	prj_model_ptr_t m_PrjModel;
	fexp_view_ptr_t	m_FexpView;
	coord_ptr_t		m_Coord;

	Impl(model_ptr_t model, prj_model_ptr_t prj_model, fexp_view_ptr_t fexp_view, coord_ptr_t coord)
		: m_Model(model)
		, m_PrjModel(prj_model)
		, m_FexpView(fexp_view)
		, m_Coord(coord)
	{ }
};	///	!struct Impl
/// Construction
FuImportPresenter::FuImportPresenter(model_ptr_t model, prj_model_ptr_t prj_model, fexp_view_ptr_t fexp_view, coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, prj_model, fexp_view, coord))
{ }

void FuImportPresenter::Init()
{
	m_Impl->m_PrjModel->SettingsFlowOut()
		.subscribe(m_Impl->m_Model->PrjSettingsFlowIn());

	m_Impl->m_FexpView->OpenFuFileFlowOut()
		.observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe(m_Impl->m_Model->FuFilepathFlowIn());
}
}	///	!namespace fusion
}	///	!namespace fu