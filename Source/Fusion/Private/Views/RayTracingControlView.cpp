#include <Views/RayTracingControlView.h>
#include <imgui.h>
namespace fu {
namespace fusion {

struct RayTracingControlView::Impl
{
	float	m_CullMin = -10.0f;
	float	m_CullMax = 10.0f;
	float	m_CullPos = 10.0f;
	float	m_PclSizeMin = 0.0f;
	float	m_PclSizeMax = 0.02f;
	float	m_PclSize = 0.01f;
	bool	m_RenderNormals = false;

	rxcpp::subjects::subject<float> m_OnCullingPlanePositionChangedSubj;
	rxcpp::subjects::subject<float> m_OnPclSizeChangedSubj;
	rxcpp::subjects::subject<bool>	m_OnRenderNormalsFlowOutSubj;
	Impl() { }
};	///	!struct Impl
/// Construction
RayTracingControlView::RayTracingControlView()
	: app::Widget("3D Viewport controls"), m_Impl(spimpl::make_unique_impl<Impl>())
{ }

void RayTracingControlView::Render()
{
	if (!this->IsActive())
		return;

	ImGui::PushItemWidth(100.0f);
	ImGui::Begin("3D Viewport Control");
	{
		if (ImGui::SliderFloat("Culling##plane y position", &m_Impl->m_CullPos, m_Impl->m_CullMin, m_Impl->m_CullMax))
		{
			m_Impl->m_OnCullingPlanePositionChangedSubj.get_subscriber().on_next(m_Impl->m_CullPos);
		}
		if (ImGui::SliderFloat("Point Size## point cloud size", &m_Impl->m_PclSize, m_Impl->m_PclSizeMin, m_Impl->m_PclSizeMax))
		{
			m_Impl->m_OnPclSizeChangedSubj.get_subscriber().on_next(m_Impl->m_PclSize);
		}
		if (ImGui::Checkbox("Render Normals", &m_Impl->m_RenderNormals))
		{
			m_Impl->m_OnRenderNormalsFlowOutSubj.get_subscriber().on_next(m_Impl->m_RenderNormals);
		}
	}
	ImGui::End();
	ImGui::PopItemWidth();
}

rxcpp::observable<float> fu::fusion::RayTracingControlView::OnCullingPlanePositionChanged()
{
	return m_Impl->m_OnCullingPlanePositionChangedSubj.get_observable().as_dynamic();
}

rxcpp::observable<float> fu::fusion::RayTracingControlView::OnPclSizeChanged()
{
	return m_Impl->m_OnPclSizeChangedSubj.get_observable().as_dynamic();
}

rxcpp::observable<bool> RayTracingControlView::OnRenderNormalsFlowOut()
{
	return m_Impl->m_OnRenderNormalsFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu