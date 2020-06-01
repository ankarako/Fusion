#include <Presenters/PerformanceImportPresenter.h>
#include <Models/PerformanceImportModel.h>
#include <Models/ViewportTracingModel.h>
#include <Core/Coordination.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief PerformanceImportPresenter implementation
struct PerformanceImportPresenter::Impl
{
	model_ptr_t 		m_Model;
	view_rt_model_ptr_t m_ViewTracingModel;
	coord_ptr_t			m_Coord;
	/// Construction
	Impl(model_ptr_t model, view_rt_model_ptr_t view_rt_model, coord_ptr_t coord)
		: m_Model(model)
		, m_ViewTracingModel(view_rt_model)
		, m_Coord(coord)
	{ }
};
///	Construction
PerformanceImportPresenter::PerformanceImportPresenter(model_ptr_t model, view_rt_model_ptr_t view_rt_model, coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, view_rt_model, coord))
{ }
///	\brief presenter initialization
void PerformanceImportPresenter::Init()
{
	///============
	/// CameraData
	///============
	/*m_Impl->m_Model->ViewportDataFlowOut()
		.observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe(m_Impl->m_ViewTracingModel->CameraDataFlowIn());*/
	///==========
	/// MeshData
	///==========
	//m_Impl->m_Model->MeshDataFlowOut()
	//	.observe_on(m_Impl->m_Coord->ModelCoordination())
	//	.subscribe(m_Impl->m_ViewTracingModel->MeshDataFlowIn());
}
}	///	!namespace mvt
}	///	!namespace fu