#include <Views/PlayerControllerView.h>
#include <FontManager.h>
#include <FontDef.h>
#include <imgui.h>

namespace fusion {
///	\struct Impl
///	\brief PlayerControllerView implementation
struct PlayerControllerView::Impl
{
	fman_ptr_t	m_FontManger;
	int m_SliderValue{ 0 };
	/// Construction
	Impl(fman_ptr_t fman) 
		: m_FontManger(fman)
	{ }
};	///	!struct Impl
/// Construction
PlayerControllerView::PlayerControllerView(fman_ptr_t fman)
	: app::Widget("PlayerController"), m_Impl(spimpl::make_unique_impl<Impl>(fman))
{ }

void PlayerControllerView::Render()
{
	ImGui::PushFont(m_Impl->m_FontManger->GetFont(app::FontManager::FontType::Regular));
	ImGui::Begin("Player");
	{
		if (ImGui::Button(ICON_MD_SKIP_PREVIOUS))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_PLAY_ARROW))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_STOP))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_SKIP_NEXT))
		{

		}
		ImGui::SameLine();
		if (ImGui::SliderInt("##playbackslider", &m_Impl->m_SliderValue, 0, 10000))
		{

		}
		
	}
	ImGui::End();
	ImGui::PopFont();
}
}	///	!namespace fusion