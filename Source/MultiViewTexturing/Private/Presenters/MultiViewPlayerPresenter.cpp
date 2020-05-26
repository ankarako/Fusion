#include <Presenters/MultiViewPlayerPresenter.h>
#include <Models/MultiViewPlayerModel.h>
#include <Models/PerformanceImportModel.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief MultiViewPlayerPresenter implementation
struct MultiViewPlayerPresenter::Impl
{
	player_model_ptr_t		m_Model;
	perf_import_model_ptr_t m_PerfImportModel;
	/// Construction
	Impl(player_model_ptr_t model, perf_import_model_ptr_t perf_import_model)
		: m_Model(model)
		, m_PerfImportModel(perf_import_model)
	{ }
};	///	!struct Impl
/// Construction
MultiViewPlayerPresenter::MultiViewPlayerPresenter(player_model_ptr_t model, perf_import_model_ptr_t perf_import_model)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, perf_import_model))
{ }
/// \brief presenter initialization
void MultiViewPlayerPresenter::Init()
{
	///========================
	/// Texture video filenames
	///========================
	m_Impl->m_PerfImportModel->VideoTextureFilepathsFlowOut()
		.subscribe(m_Impl->m_Model->VideoFilepathsFlowIn());
}
}	///	!namespace mvt
}	///	!namespace fu