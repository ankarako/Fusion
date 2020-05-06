#include <Presenters/NormalsResultPresenter.h>
#include <Models/NormalsEstimationModel.h>
#include <Views/NormalsResultView.h>
#include <Core/Coordination.h>

namespace fu {
namespace fusion {

struct NormalsResultPresenter::Impl
{
	view_ptr_t	m_View;
	model_ptr_t m_Model;
	coord_ptr_t m_Coordination;

	Impl(view_ptr_t view, model_ptr_t model, coord_ptr_t coord)
		: m_View(view), m_Model(model), m_Coordination(coord)
	{ }
};	///	!struct Impl
/// Construction
NormalsResultPresenter::NormalsResultPresenter(view_ptr_t view, model_ptr_t model, coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, model, coord))
{ }

void NormalsResultPresenter::Init()
{
	///=====================
	/// normal img flow out
	///=====================
	m_Impl->m_Model->NormalsBufferRGBAFlowOut().observe_on(m_Impl->m_Coordination->UICoordination())
		.subscribe(m_Impl->m_View->NormalsImageFlowIn());
}
}	///	!namespace fusion
}	///	!namespace fu