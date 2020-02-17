#include <Views/PlayerControllerView.h>
#include <imgui.h>
namespace fusion {
///	\struct Impl
///	\brief player viewport widget implementation
struct PlayerViewportView::Impl
{
	Impl() { }
};	///	!struct Impl
///	Construction
PlayerViewportView::PlayerViewportView()
	: app::Widget("Player Viewport"), m_Impl(spimpl::make_unique_impl<Impl>())
{ }
///	\brief widget rendering
void PlayerViewportView::Render()
{
	ImGui::Begin("Video Viewport");
	{

	}
	ImGui::End();

}
}	///	!namespace fusion