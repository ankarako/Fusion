#include <Views/PlayerViewportView.h>
#include <FontManager.h>
#include <imgui.h>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief player viewport widget implementation
struct PlayerViewportView::Impl
{
	fman_ptr_t	m_FontManager;

	ImVec2		m_WindowSize{ 0.0f, 0.0f };

	rxcpp::subjects::subject<float> OnViewportWidthChangedSubj;
	rxcpp::subjects::subject<float> OnViewportHeightChangedSubj;
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
	ImVec2 winSize = ImGui::GetWindowSize();
	if (winSize.x != m_Impl->m_WindowSize.x)
	{
		m_Impl->m_WindowSize.x = winSize.x;
		m_Impl->OnViewportWidthChangedSubj.get_subscriber().on_next(m_Impl->m_WindowSize.x);
	}
	
	if (winSize.y != m_Impl->m_WindowSize.y)
	{
		m_Impl->m_WindowSize.y = winSize.y;
		m_Impl->OnViewportHeightChangedSubj.get_subscriber().on_next(m_Impl->m_WindowSize.y);
	}
	
	ImGui::PushFont(m_Impl->m_FontManager->GetFont(app::FontManager::FontType::Regular));
	ImGui::Begin("Video Viewport");
	{

	}
	ImGui::End();
	ImGui::PopFont();
}

rxcpp::observable<float> PlayerViewportView::OnViewportWidthChanged()
{
	return m_Impl->OnViewportWidthChangedSubj.get_observable().as_dynamic();
}
rxcpp::observable<float> PlayerViewportView::OnViewportHeightChanged()
{
	return m_Impl->OnViewportHeightChangedSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu