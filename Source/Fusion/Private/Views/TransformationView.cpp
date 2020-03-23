#include <Views/TransformationView.h>
#include <imgui.h>

namespace fu {
namespace fusion {

	struct TransformationView::Impl
	{
		float m_TransCur[3]		= { 0.0f, 0.0f, 0.0f };
		float m_TransPrev[3]	= { 0.0f, 0.0f, 0.0f };
		float m_RotCur[3]		= { 0.0f, 0.0f, 0.0f };
		float m_RotPrev[3]		= { 0.0f, 0.0f, 0.0f };
		float m_ScaleCur		= 1.0f;
		float m_ScalePrev		= 1.0f;
		float m_OutputScale = 0.0f;
		trans_vec_t	m_OutputTrans;
		trans_vec_t	m_OutputRot;
		float m_TransMin{ -100.0f };
		float m_TransMax{ 100.0f };
		float m_RotMin{ -180.0f };
		float m_RotMax{ 180.0f };
		float m_ScaleMin{ 0.001f };
		float m_ScaleMax{ 10.0f };

	rxcpp::subjects::subject<trans_vec_t>	m_TranslationFlowOutSubj;
	rxcpp::subjects::subject<trans_vec_t>	m_RotationFlowOutSubj;
	rxcpp::subjects::subject<float>			m_UniforScaleFlowOutSubj;
	rxcpp::subjects::subject<trans_t>		m_TransformFlowOutSubj;
	Impl() { }
};	///	!struct Impl
/// Construction
TransformationView::TransformationView()
	: app::Widget("Translation"), m_Impl(spimpl::make_unique_impl<Impl>())
{ }

void TransformationView::Render()
{
	bool isActive = this->IsActive();
	if (!isActive)
		return;
	auto flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize;
	ImGui::Begin("Translation", &isActive, flags);
	{
		ImGui::PushItemWidth(300.0f);
		if (ImGui::SliderFloat3("Translation", m_Impl->m_TransCur, m_Impl->m_TransMin, m_Impl->m_TransMax, "%.3f", 2.0f))
		{
			m_Impl->m_OutputTrans =
			{
				m_Impl->m_TransCur[0] - m_Impl->m_TransPrev[0],
				m_Impl->m_TransCur[1] - m_Impl->m_TransPrev[1],
				m_Impl->m_TransCur[2] - m_Impl->m_TransPrev[2]
			};
			std::memcpy(m_Impl->m_TransPrev, m_Impl->m_TransCur, 3 * sizeof(float));
			m_Impl->m_TranslationFlowOutSubj.get_subscriber().on_next(m_Impl->m_OutputTrans);
		}
		if (ImGui::SliderFloat3("Rotation", m_Impl->m_RotCur, m_Impl->m_RotMin, m_Impl->m_RotMax, "%.3f", 1.5f))
		{
			m_Impl->m_OutputRot =
			{
				m_Impl->m_RotCur[0] - m_Impl->m_RotPrev[0],
				m_Impl->m_RotCur[1] - m_Impl->m_RotPrev[1],
				m_Impl->m_RotCur[2] - m_Impl->m_RotPrev[2]
			};
			std::memcpy(m_Impl->m_RotPrev, m_Impl->m_RotCur, 3 * sizeof(float));
			m_Impl->m_RotationFlowOutSubj.get_subscriber().on_next(m_Impl->m_OutputRot);
		}
		if (ImGui::SliderFloat("Uniform Scale", &m_Impl->m_ScaleCur, m_Impl->m_ScaleMin, m_Impl->m_ScaleMax))
		{
			m_Impl->m_OutputScale = 1.0f + m_Impl->m_ScaleCur - m_Impl->m_ScalePrev;
			m_Impl->m_ScalePrev = m_Impl->m_ScaleCur;
			m_Impl->m_UniforScaleFlowOutSubj.get_subscriber().on_next(m_Impl->m_OutputScale);
		}
		ImGui::PopItemWidth();
		if (ImGui::Button("Reset"))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			this->Deactivate();
		}
	}
	ImGui::End();
}

rxcpp::observable<TransformationView::trans_vec_t> fu::fusion::TransformationView::TranslationVectorFlowOut()
{
	return m_Impl->m_TranslationFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<TransformationView::trans_vec_t> fu::fusion::TransformationView::RotationVectorFlowOut()
{
	return m_Impl->m_RotationFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<float> fu::fusion::TransformationView::UniformScaleFlowOut()
{
	return m_Impl->m_UniforScaleFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<TransformationView::trans_t> fu::fusion::TransformationView::TransformFlowOut()
{
	return m_Impl->m_TransformFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu