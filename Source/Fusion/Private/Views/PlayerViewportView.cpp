#include <Views/PlayerViewportView.h>
#include <FontManager.h>
#include <imgui.h>
namespace fusion {
///	\struct Impl
///	\brief player viewport widget implementation
struct PlayerViewportView::Impl
{
	fman_ptr_t	m_FontManager;
	Impl(fman_ptr_t fman)
		: m_FontManager(fman)
	{ }
};	///	!struct Impl
///	Construction
PlayerViewportView::PlayerViewportView(fman_ptr_t fman)
	: app::Widget("Player Viewport"), m_Impl(spimpl::make_unique_impl<Impl>(fman))
{ }
///	\brief widget rendering
void PlayerViewportView::Render()
{
	ImGui::PushFont(m_Impl->m_FontManager->GetFont(app::FontManager::FontType::Regular));
	ImGui::Begin("Video Viewport");
	{

	}
	ImGui::End();
	ImGui::PopFont();
}
}	///	!namespace fusion