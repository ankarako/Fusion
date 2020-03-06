#include <Presenters/PlayerViewportPresenter.h>
#include <Views/PlayerViewportView.h>
#include <Models/PlayerModel.h>
#include <Models/VideoTracingModel.h>
#include <WidgetRepo.h>
#include <Core/Coordination.h>

namespace fu {
namespace fusion {
struct PlayerViewportPresenter::Impl
{
	model_ptr_t			m_Model;
	view_ptr_t			m_View;
	tracer_model_ptr_t	m_TracerModel;
	wrepo_ptr_t			m_Wrepo;
	coord_ptr_t			m_Coord;

	Impl(model_ptr_t model, view_ptr_t view, tracer_model_ptr_t tracer_model, wrepo_ptr_t wrepo, coord_ptr_t coord)
		: m_Model(model), m_View(view), m_TracerModel(tracer_model), m_Wrepo(wrepo), m_Coord(coord)
	{ }
};
/// Construction
PlayerViewportPresenter::PlayerViewportPresenter(model_ptr_t model, view_ptr_t view, tracer_model_ptr_t tracer_model, wrepo_ptr_t wrepo, coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, view, tracer_model, wrepo, coord))
{ }

void PlayerViewportPresenter::Init()
{
	/// Widget activation task
	m_Impl->m_View->OnActivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_View);
	});
	/// Widget deactivation task
	m_Impl->m_View->OnDeactivated().subscribe(
		[this](auto _)
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_View);
	});
	/// frame width subscription
	m_Impl->m_Model->FrameWidthFlowOut()
		.subscribe(m_Impl->m_View->FrameWidthFlowIn());
	/// frame height subscription
	m_Impl->m_Model->FrameHeightFlowOut()
		.subscribe(m_Impl->m_View->FrameHeightFlowIn());
	/// frame size subscription
	m_Impl->m_Model->FrameSizeFlowOut()
		.subscribe(m_Impl->m_TracerModel->FrameSizeFlowIn());
	///	viewport width changed task
	m_Impl->m_View->OnViewportWidthChanged().subscribe(
		[this](float newWidth)
	{
		
	});
	/// frame flow out from decoder task
	m_Impl->m_Model->CurrentFrameFlowOut().observe_on(m_Impl->m_Coord->UICoordination())
		.subscribe(m_Impl->m_TracerModel->FrameFlowIn());
	/// frame flow out from tracer task
	m_Impl->m_TracerModel->FrameFlowOut().observe_on(m_Impl->m_Coord->UICoordination())
		.subscribe(m_Impl->m_View->FrameFlowIn());
	m_Impl->m_View->Activate();
}
}	///	!namespace fusion
}	///	!namespace fu