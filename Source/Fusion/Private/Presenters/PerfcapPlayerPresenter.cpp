#include <Presenters/PerfcapPlayerPresenter.h>
#include <Models/PerfcapPlayerModel.h>
#include <Models/RayTracingModel.h>
#include <Models/FuImportModel.h>
#include <Models/VideoTracingModel.h>
#include <Core/Coordination.h>
#include <Views/SequencerView.h>

namespace fu {
namespace fusion {

struct PerfcapPlayerPresenter::Impl
{
	model_ptr_t				m_Model;
	rt_model_ptr_t			m_RayTracingModel;
	seq_view_ptr_t			m_SequencerView;
	fuimport_model_ptr_t	m_FuImportModel;
	vrt_model_ptr_t			m_VideoTracingModel;
	coord_ptr_t				m_Coordination;
	/// Construction
	Impl(model_ptr_t model, rt_model_ptr_t rt_model, seq_view_ptr_t seq_view, fuimport_model_ptr_t fuimport_model, vrt_model_ptr_t vrt_model, coord_ptr_t coord)
		: m_Model(model), m_RayTracingModel(rt_model), m_SequencerView(seq_view), m_FuImportModel(fuimport_model), m_VideoTracingModel(vrt_model), m_Coordination(coord)
	{ }
};	///	!struct Impl
/// Construction
PerfcapPlayerPresenter::PerfcapPlayerPresenter(model_ptr_t model, rt_model_ptr_t rt_model, seq_view_ptr_t seq_view, fuimport_model_ptr_t fuimport_model, vrt_model_ptr_t vrt_model, coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, rt_model, seq_view, fuimport_model, vrt_model, coord))
{ }

void PerfcapPlayerPresenter::Init()
{
	///=====================
	/// Perfcap data flow in
	///=====================
	m_Impl->m_FuImportModel->PerfcapMeshDataFlowOut()
		.subscribe(m_Impl->m_Model->PerfcapMeshDataFlowIn());
	///=======================
	/// Template mesh flow out
	///=======================
	m_Impl->m_Model->TemplateMeshDataFlowOut()
		.subscribe(m_Impl->m_RayTracingModel->TemplateMeshDataFlowIn());
	m_Impl->m_Model->TemplateMeshDataFlowOut()
		.subscribe(m_Impl->m_VideoTracingModel->TemplateMeshDataFlowIn());
	///============================
	/// Animated Mesh Data Flow Out
	///============================
	m_Impl->m_Model->AnimatedMeshDataFlowOut()
		//.observe_on(m_Impl->m_Coordination->ModelCoordination())
		.subscribe(m_Impl->m_RayTracingModel->AnimatedMeshDataFlowIn());

	m_Impl->m_Model->AnimatedMeshDataFlowOut()
		//.observe_on(m_Impl->m_Coordination->ModelCoordination())
		.subscribe(m_Impl->m_VideoTracingModel->AnimatedMeshDataFlowIn());
}
}	///	!namespace fusion
}	///	!namespace fu