#include <Views/PlayerViewportView.h>
#include <Buffer.h>
#include <FontManager.h>
#include <imgui.h>
#include <GL/gl3w.h>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief player viewport widget implementation
struct PlayerViewportView::Impl
{
	/// font manager
	fman_ptr_t	m_FontManager;
	/// viewport size
	ImVec2		m_WindowSize{ 0.0f, 0.0f };
	/// gl texture width
	int m_DisplayTextureWidth{ 0 };
	///	gl texture height
	int m_DisplayTextureHeight{ 0 };
	/// gl texture to display to viewport
	GLuint m_DisplayTexture;
	///	inputs
	///	frame input
	rxcpp::subjects::subject<BufferCPU<uchar4>> m_FrameFlowInSubj;
	///	frame width input
	rxcpp::subjects::subject<int>	m_FrameWidthFlowInSubj;
	///	frame heght input
	rxcpp::subjects::subject<INT>	m_FrameHeightFlowinSubj;
	///	outputs
	///	width changed event
	rxcpp::subjects::subject<float> OnViewportWidthChangedSubj;
	///	height changed event
	rxcpp::subjects::subject<float> OnViewportHeightChangedSubj;
	/// Construction
	Impl(fman_ptr_t fman)
		: m_FontManager(fman)
	{ }
};	///	!struct Impl
///	Construction
PlayerViewportView::PlayerViewportView(fman_ptr_t fman)
	: app::Widget("Player Viewport"), m_Impl(spimpl::make_unique_impl<Impl>(fman))
{ }
/// \brief initialize the widget
void PlayerViewportView::Init()
{
	/// frame width flow in task
	m_Impl->m_FrameWidthFlowInSubj.get_observable().subscribe(
		[this](int width) 
	{
		m_Impl->m_DisplayTextureWidth = width;
	});
	/// frame height flow in task
	m_Impl->m_FrameHeightFlowinSubj.get_observable().subscribe(
		[this](int height) 
	{
		m_Impl->m_DisplayTextureHeight = height;
	});
	/// both width height task, create g; texture where we can save our
	/// frame for displaying it with imgui
	m_Impl->m_FrameWidthFlowInSubj.get_observable()
		.with_latest_from(m_Impl->m_FrameHeightFlowinSubj.get_observable())
		.subscribe([this](auto wh) 
	{
		m_Impl->m_DisplayTextureWidth = std::get<0>(wh);
		m_Impl->m_DisplayTextureHeight = std::get<1>(wh);
		/// create a gl texture
		glGenTextures(1, &m_Impl->m_DisplayTexture);
		/// bind the texture for setup
		glBindTexture(GL_TEXTURE_2D, m_Impl->m_DisplayTexture);
		/// set min filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	});
	/// frame flow in task
	m_Impl->m_FrameFlowInSubj.get_observable().observe_on(rxcpp::observe_on_event_loop())
		.subscribe([this](BufferCPU<uchar4> frame) 
	{
		glBindTexture(GL_TEXTURE_2D, m_Impl->m_DisplayTexture);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Impl->m_DisplayTextureWidth, m_Impl->m_DisplayTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame->Data());
	});
	/// create an initial texture with the current 
	/// frame flow in task

}
///	\brief widget rendering
void PlayerViewportView::Render()
{
	bool isActive = this->IsActive();
	if (!isActive)
		return;

	auto winFlags = ImGuiWindowFlags_NoScrollbar;

	ImGui::PushFont(m_Impl->m_FontManager->GetFont(app::FontManager::FontType::Regular));
	ImGui::Begin("Video Viewport", &isActive, winFlags);
	{
		ImVec2 winSize = ImGui::GetWindowSize();
		if (winSize.x != m_Impl->m_WindowSize.x)
		{
			m_Impl->m_WindowSize.x = winSize.x;
			m_Impl->m_WindowSize.y = m_Impl->m_WindowSize.x / 2;
			m_Impl->OnViewportWidthChangedSubj.get_subscriber().on_next(m_Impl->m_WindowSize.x);
			m_Impl->OnViewportHeightChangedSubj.get_subscriber().on_next(m_Impl->m_WindowSize.y);
			ImGui::SetWindowSize(m_Impl->m_WindowSize);
		}

		if (winSize.y != m_Impl->m_WindowSize.y)
		{
			m_Impl->m_WindowSize.y = winSize.y;
			m_Impl->m_WindowSize.x = 2 * m_Impl->m_WindowSize.y;
			m_Impl->OnViewportHeightChangedSubj.get_subscriber().on_next(m_Impl->m_WindowSize.y);
			m_Impl->OnViewportWidthChangedSubj.get_subscriber().on_next(m_Impl->m_WindowSize.x);
			ImGui::SetWindowSize(m_Impl->m_WindowSize);
		}
		if (m_Impl->m_DisplayTextureWidth != 0 && m_Impl->m_DisplayTextureHeight != 0)
		{
			ImGui::Image((void*)(intptr_t)m_Impl->m_DisplayTexture, m_Impl->m_WindowSize);
		}
	}
	ImGui::End();
	ImGui::PopFont();
}
rxcpp::observer<BufferCPU<uchar4>> fu::fusion::PlayerViewportView::FrameFlowIn()
{
	return m_Impl->m_FrameFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<int> fu::fusion::PlayerViewportView::FrameWidthFlowIn()
{
	return m_Impl->m_FrameWidthFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<int> fu::fusion::PlayerViewportView::FrameHeightFlowIn()
{
	return m_Impl->m_FrameHeightFlowinSubj.get_subscriber().get_observer().as_dynamic();
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