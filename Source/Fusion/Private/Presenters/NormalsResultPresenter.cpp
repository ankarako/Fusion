#include <Presenters/NormalsResultPresenter.h>
#include <Models/NormalsEstimationModel.h>
#include <Models/RayTracingModel.h>
#include <Views/NormalsResultView.h>
#include <Core/Coordination.h>

namespace fu {
namespace fusion {

struct NormalsResultPresenter::Impl
{
	view_ptr_t		m_View;
	model_ptr_t		m_Model;
	coord_ptr_t		m_Coordination;
	rt_model_ptr_t	m_RtModel;

	Impl(view_ptr_t view, model_ptr_t model, coord_ptr_t coord, rt_model_ptr_t rt_model)
		: m_View(view), m_Model(model), m_Coordination(coord), m_RtModel(rt_model)
	{ }
};	///	!struct Impl
/// Construction
NormalsResultPresenter::NormalsResultPresenter(view_ptr_t view, model_ptr_t model, coord_ptr_t coord, rt_model_ptr_t rt_model)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, model, coord, rt_model))
{ }

void NormalsResultPresenter::Init()
{
	///=====================
	/// normal img flow out
	///=====================
	m_Impl->m_Model->NormalsBufferRGBAFlowOut().observe_on(m_Impl->m_Coordination->UICoordination())
		.subscribe(m_Impl->m_View->NormalsImageFlowIn());
	m_Impl->m_Model->NormalsBufferFloatFlowOut().observe_on(m_Impl->m_Coordination->ModelCoordination())
		.subscribe(m_Impl->m_RtModel->PointcloudNormalsFlowIn());
}
}	///	!namespace fusion
}	///	!namespace fu