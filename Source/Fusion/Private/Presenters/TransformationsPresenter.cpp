#include <Presenters/TransformationsPresenter.h>
#include <Views/SequencerView.h>
#include <Views/TransformationView.h>
#include <Models/RayTracingModel.h>
#include <Models/VideoTracingModel.h>
#include <WidgetRepo.h>

namespace fu {
namespace fusion {

struct TransformationsPresenter::Impl
{
	seq_view_ptr_t		m_SequencerView;
	trans_view_ptr_t	m_TransformationView;
	rt_model_ptr_t		m_RayTracingModel;
	vrt_model_ptr_t		m_VideoTracingModel;
	wrepo_ptr_t			m_Wrepo;

	Impl(seq_view_ptr_t seq_view, trans_view_ptr_t trans_view, rt_model_ptr_t rt_model, vrt_model_ptr_t vrt_model, wrepo_ptr_t wrepo)
		: m_SequencerView(seq_view)
		, m_TransformationView(trans_view)
		, m_RayTracingModel(rt_model)
		, m_VideoTracingModel(vrt_model)
		, m_Wrepo(wrepo)
	{ }
};	///	!struct Impl
/// Construction
TransformationsPresenter::TransformationsPresenter(seq_view_ptr_t seq_view, trans_view_ptr_t trans_view, rt_model_ptr_t rt_model, vrt_model_ptr_t vrt_model, wrepo_ptr_t wrepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(seq_view, trans_view, rt_model, vrt_model, wrepo))
{ }

void TransformationsPresenter::Init()
{
	///======================
	///	Trans view acitvation
	///======================
	m_Impl->m_TransformationView->OnActivated()
		.subscribe([this](auto _) 
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_TransformationView);
	});
	///========================
	///	trans view deactivation
	///========================
	m_Impl->m_TransformationView->OnDeactivated()
		.subscribe([this](auto _) 
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_TransformationView);
	});
	///===================================
	/// sequencer translation clicked task
	///===================================
	m_Impl->m_SequencerView->OnTranslationClicked()
		.subscribe([this](auto _) 
	{
		if (!m_Impl->m_TransformationView->IsActive())
		{
			m_Impl->m_TransformationView->Activate();
		}
		else
		{
			m_Impl->m_TransformationView->Deactivate();
		}
	});
	///======================
	/// Translation flow out
	///======================
	m_Impl->m_TransformationView->TranslationVectorFlowOut()
		.subscribe(m_Impl->m_RayTracingModel->PerfcapTranslationFlowIn());
	m_Impl->m_TransformationView->TranslationVectorFlowOut()
		.subscribe(m_Impl->m_VideoTracingModel->PerfcapTranslationFlowIn());
	///===================
	///	rotation flow out
	///===================
	m_Impl->m_TransformationView->RotationVectorFlowOut()
		.subscribe(m_Impl->m_RayTracingModel->PerfcapRotationFlowIn());
	m_Impl->m_TransformationView->RotationVectorFlowOut()
		.subscribe(m_Impl->m_VideoTracingModel->PerfcapRotationFlowIn());
	///==================
	/// scale flow out
	///==================
	m_Impl->m_TransformationView->UniformScaleFlowOut()
		.subscribe(m_Impl->m_RayTracingModel->PerfcapScaleFlowIn());
	m_Impl->m_TransformationView->UniformScaleFlowOut()
		.subscribe(m_Impl->m_RayTracingModel->PerfcapScaleFlowIn());
}
}	///	!namespace fusion
}	///	!namespace fu