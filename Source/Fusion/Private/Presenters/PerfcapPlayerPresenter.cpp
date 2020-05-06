#include <Presenters/PerfcapPlayerPresenter.h>
#include <Models/PerfcapPlayerModel.h>
#include <Models/RayTracingModel.h>
#include <Views/SequencerView.h>

namespace fu {
namespace fusion {

struct PerfcapPlayerPresenter::Impl
{
	model_ptr_t		m_Model;
	rt_model_ptr_t	m_RayTracingModel;
	seq_view_ptr_t	m_SequencerView;
	/// Construction
	Impl(model_ptr_t model, rt_model_ptr_t rt_model, seq_view_ptr_t seq_view)
		: m_Model(model), m_RayTracingModel(rt_model), m_SequencerView(seq_view)
	{ }
};	///	!struct Impl
/// Construction
PerfcapPlayerPresenter::PerfcapPlayerPresenter(model_ptr_t model, rt_model_ptr_t rt_model, seq_view_ptr_t seq_view)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, rt_model, seq_view))
{ }

void PerfcapPlayerPresenter::Init()
{
	///==========================
	/// Template mesh output task
	///===========================
	m_Impl->m_Model->TemplateMeshDataFlowOut()
		.subscribe(m_Impl->m_RayTracingModel->MeshDataFlowIn());
}
}	///	!namespace fusion
}	///	!namespace fu