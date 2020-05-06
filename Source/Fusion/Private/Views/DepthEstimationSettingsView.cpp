#include <Views/DepthEstimationSettingsView.h>
#include <imgui.h>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief
struct DepthEstimationSettingsView::Impl
{
	rxcpp::subjects::subject<DepthEstimationModels> m_OnDepthEstimationModelChangedSubj;
	rxcpp::subjects::subject<void*>					m_OnEstimateDepthButtonClickedSubj;
	rxcpp::subjects::subject<void*>					m_OnCancelButtonClickedSubj;

	DepthEstimationModels	m_SeletectedModel{ DepthEstimationModels::UResNet };

	/// Construction
	Impl() { }
};
///	Construction
DepthEstimationSettingsView::DepthEstimationSettingsView()
	: app::Widget("Depth Estimation Settings"), m_Impl(spimpl::make_unique_impl<Impl>())
{ }

void DepthEstimationSettingsView::Render()
{
	if (!this->IsActive())
		return;

	ImGui::Begin("Depth Estimation Settings");
	{
		ImGui::PushItemWidth(200.0f);
		if (ImGui::BeginCombo("##modelcombo", DepthEstModel2Str.at(m_Impl->m_SeletectedModel).c_str()))
		{
			for (int model = 0; model < k_DepthEstimationModels.size(); model++)
			{
				bool currentSelectedModel = m_Impl->m_SeletectedModel == k_DepthEstimationModels[model];
				if (ImGui::Selectable(DepthEstModel2Str.at(k_DepthEstimationModels[model]).c_str()), currentSelectedModel)
				{
					m_Impl->m_SeletectedModel = k_DepthEstimationModels[model];
					m_Impl->m_OnDepthEstimationModelChangedSubj.get_subscriber().on_next(m_Impl->m_SeletectedModel);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		if (ImGui::Button("Estimate Depth"))
		{
			m_Impl->m_OnEstimateDepthButtonClickedSubj.get_subscriber().on_next(nullptr);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel##depthestset"))
		{
			m_Impl->m_OnCancelButtonClickedSubj.get_subscriber().on_next(nullptr);
		}
	}
	ImGui::End();
}

rxcpp::observable<DepthEstimationModels> fu::fusion::DepthEstimationSettingsView::OnDepthEstimationModelChanged()
{
	return m_Impl->m_OnDepthEstimationModelChangedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fu::fusion::DepthEstimationSettingsView::OnEstimateDepthButtonClicked()
{
	return m_Impl->m_OnEstimateDepthButtonClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fu::fusion::DepthEstimationSettingsView::OnCancelButtonClicked()
{
	return m_Impl->m_OnCancelButtonClickedSubj.get_observable().as_dynamic();
}
}	/// !namespace fusion
}	///	!namespace fu