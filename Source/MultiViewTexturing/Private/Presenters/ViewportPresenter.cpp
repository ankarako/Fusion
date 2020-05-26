#include <Presenters/ViewportPresenter.h>
#include <Views/ViewportView.h>
#include <Models/ViewportTracingModel.h>
#include <WidgetRepo.h>
#include <Core/Coordination.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief ViewportPresenter initialization
struct ViewportPresenter::Impl
{
	view_ptr_t 	m_View;
	model_ptr_t m_Model;
	wrepo_ptr_t	m_Wrepo;
	coord_ptr_t m_Coord;
	/// Construction
	Impl(view_ptr_t view, model_ptr_t model, wrepo_ptr_t wrepo, coord_ptr_t coord)
		: m_View(view)
		, m_Model(model)
		, m_Wrepo(wrepo)
		, m_Coord(coord)
	{ }
};	///	!struct Impl
/// Construction
ViewportPresenter::ViewportPresenter(view_ptr_t view, model_ptr_t model, wrepo_ptr_t wrepo, coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, model, wrepo, coord))
{ }
/// \brief presenter initialization
void ViewportPresenter::Init()
{
	///===============================
	/// Window activation/deactivation
	///===============================
	m_Impl->m_View->OnActivated()
		.subscribe([this](auto _)
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_View);
	});
	m_Impl->m_View->OnDeactivated()
		.subscribe([this](auto _)
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_View);
	});
	///============================
	/// Viewport Size modification
	///============================
	m_Impl->m_View->ViewportSizeFlowOut()
		.subscribe(m_Impl->m_Model->ViewportSizeFlowIn());
	///===========================
	/// Frame Buffer Visualization
	///===========================
	m_Impl->m_Model->FrameBufferFlowOut()
		.observe_on(m_Impl->m_Coord->UICoordination())
		.subscribe(m_Impl->m_View->FrameBufferFlowIn());
	///========================
	/// Camera transformations
	///=========================
	m_Impl->m_View->RotationTransformFlowOut()
		.subscribe(m_Impl->m_Model->CameraRotationTransformFlowIn());
	m_Impl->m_View->TranslationVectorFlowOut()
		.subscribe(m_Impl->m_Model->CameraTranslationVectorFlowIn());
	/// Activate the view
	m_Impl->m_View->Activate();
}
}	///	!namespace mvt
}	///	!namespace fu