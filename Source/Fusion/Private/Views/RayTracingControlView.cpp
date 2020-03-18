#include <Views/RayTracingControlView.h>
#include <imgui.h>
namespace fu {
namespace fusion {

struct RayTracingControlView::Impl
{
	float m_CullMin = -10.0f;
	float m_CullMax = 10.0f;
	float m_CullPos = 10.0f;

	rxcpp::subjects::subject<float> m_OnCullingPlanePositionChangedSubj;
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

	ImGui::Begin("3D Viewport Control");
	{
		if (ImGui::SliderFloat("Culling##plane y position", &m_Impl->m_CullPos, m_Impl->m_CullMin, m_Impl->m_CullMax))
		{
			m_Impl->m_OnCullingPlanePositionChangedSubj.get_subscriber().on_next(m_Impl->m_CullPos);
		}
	}
	ImGui::End();
}

rxcpp::observable<float> fu::fusion::RayTracingControlView::OnCullingPlanePositionChanged()
{
	return m_Impl->m_OnCullingPlanePositionChangedSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu