#include <Views/PlayerControllerView.h>

#include <imgui.h>

namespace fusion {
///	\struct Impl
///	\brief PlayerControllerView implementation
struct PlayerControllerView::Impl
{
	int m_SliderValue{ 0 };
	/// Construction
	Impl() { }
};	///	!struct Impl
/// Construction
PlayerControllerView::PlayerControllerView()
	: app::Widget("PlayerController"), m_Impl(spimpl::make_unique_impl<Impl>())
{ }

void PlayerControllerView::Render()
{
	ImGui::Begin("Player");
	{
		if (ImGui::Button("SeekBack"))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button("Play"))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button("SeekFor"))
		{

		}
		ImGui::SameLine();
		if (ImGui::SliderInt("##playbackslider", &m_Impl->m_SliderValue, 0, 10000))
		{

		}
		
	}
	ImGui::End();
}
}	///	!namespace fusion