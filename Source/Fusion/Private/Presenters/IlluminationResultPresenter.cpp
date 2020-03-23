#include <Presenters/IlluminationResultPresenter.h>
#include <Views/IlluminationResultView.h>
#include <Models/IlluminationEstimationModel.h>
#include <Core/Coordination.h>

namespace fu {
namespace fusion {

struct IlluminationResultPresenter::Impl
{
	view_ptr_t	m_View;
	model_ptr_t	m_Model;
	coord_ptr_t m_Coordination;

	Impl(view_ptr_t view, model_ptr_t model, coord_ptr_t coord)
		: m_View(view)
		, m_Model(model)
		, m_Coordination(coord)
	{ }
};	///	!struct Impl
/// Construction
IlluminationResultPresenter::IlluminationResultPresenter(view_ptr_t view, model_ptr_t model, coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, model, coord))
{ }

void IlluminationResultPresenter::Init()
{
	m_Impl->m_Model->IlluminationMapFlowOut().observe_on(m_Impl->m_Coordination->UICoordination())
		.subscribe(m_Impl->m_View->IlluminationMapFlowIn());
}
}	///	!namespace fusion
}	///	!namespace fu