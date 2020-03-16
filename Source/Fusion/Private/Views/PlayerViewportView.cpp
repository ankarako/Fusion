#include <Views/PlayerViewportView.h>
#include <Core/Coordination.h>
#include <Buffer.h>
#include <FontManager.h>
#include <imgui.h>
#include <GL/gl3w.h>
#include <plog/Log.h>

#include <chrono>

// FIXME: delete
// #include <opencv2/core.hpp>
// #include <opencv2/imgcodecs.hpp>
// #include <opencv2/imgproc.hpp>
// #include <opencv2/videoio.hpp>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief player viewport widget implementation
struct PlayerViewportView::Impl
{
	/// font manager
	fman_ptr_t	m_FontManager;
	///
	coord_ptr_t	m_Coord;
	/// viewport size
	ImVec2		m_WindowSize{ 720.0f, 370.0f};
	/// gl texture width
	int m_DisplayTextureWidth{ 0 };
	///	gl texture height
	int m_DisplayTextureHeight{ 0 };
	/// viewport's aspect ratio
	float m_DisplayAspectRatio{ 0.0f };
	///	gl pixel buffer with texture
	GLuint m_PixelBufferHandle{ 0 };
	/// gl texture to display to viewport
	GLuint m_TextureHandle{ 0 };
	///	inputs
	///	frame input
	rxcpp::subjects::subject<BufferCPU<uchar4>> m_FrameFlowInSubj;
	///	frame width input
	rxcpp::subjects::subject<int>	m_FrameWidthFlowInSubj;
	///	frame heght input
	rxcpp::subjects::subject<int>	m_FrameHeightFlowinSubj;
	rxcpp::subjects::subject<uint2>	m_FrameSizeFlowInSubj;
	///	outputs
	///	width changed event
	rxcpp::subjects::subject<float> m_OnViewportWidthChangedSubj;
	///	height changed event
	rxcpp::subjects::subject<float> m_OnViewportHeightChangedSubj;
	/// window size changed
	rxcpp::subjects::subject<float2> m_OnViewportSizeChangedSubj;
	/// pixel buffer flow out event
	rxcpp::subjects::subject<GLuint> m_PixelBufferFlowOutSubj;

	std::chrono::steady_clock::time_point m_FirstFrameTime;
	/// wdeo siz
	/// Construction
	Impl(fman_ptr_t fman, coord_ptr_t coord)
		: m_FontManager(fman)
		, m_Coord(coord)
	{ }
};	///	!struct Impl
///	Construction
PlayerViewportView::PlayerViewportView(fman_ptr_t fman, coord_ptr_t coord)
	: app::Widget("Player Viewport"), m_Impl(spimpl::make_unique_impl<Impl>(fman, coord))
{ }
/// \brief initialize the widget
void PlayerViewportView::Init()
{
	///========================
	/// frame size flow in task
	///
	///	Setup gl pixel buffer and gl texture
	///	for displaying the video
	///========================
	m_Impl->m_FrameSizeFlowInSubj.get_observable()
		.subscribe([this](const uint2& size)
	{
		m_Impl->m_DisplayTextureWidth = size.x;
		m_Impl->m_DisplayTextureHeight = size.y;
		m_Impl->m_DisplayAspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
		if (m_Impl->m_DisplayTextureWidth != m_Impl->m_WindowSize.x || m_Impl->m_DisplayTextureHeight != m_Impl->m_WindowSize.y)
		{
			m_Impl->m_WindowSize.x = m_Impl->m_WindowSize.y * m_Impl->m_DisplayAspectRatio;
			//m_Impl->m_OnViewportSizeChangedSubj.get_subscriber().on_next(make_float2((unsigned int)m_Impl->m_WindowSize.x, (unsigned int)m_Impl->m_WindowSize.y));
		}
		/// generate a gl texture object
		glGenTextures(1, &m_Impl->m_TextureHandle);
		if (m_Impl->m_TextureHandle == 0)
		{
			LOG_ERROR << "Failed to create texture handle for video texture";
		}
		/// bind the texture to gl state
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Impl->m_TextureHandle);
		/// set magnification and minimization filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		/// unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
		m_Impl->m_PixelBufferFlowOutSubj.get_subscriber().on_next(m_Impl->m_PixelBufferHandle);
	});
	///=======================
	/// New Frame flow in Task
	///=======================
	m_Impl->m_FirstFrameTime = std::chrono::high_resolution_clock::now();
	m_Impl->m_FrameFlowInSubj.get_observable()
		.subscribe([this](BufferCPU<uchar4>& frame) 
	{
		/// bind our gl texture to gl state
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Impl->m_TextureHandle);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Impl->m_DisplayTextureWidth, m_Impl->m_DisplayTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame->Data());
		/// unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	});
}
///	\brief widget rendering
void PlayerViewportView::Render()
{
	bool isActive = this->IsActive();
	if (!isActive)
		return;

	//ImGui::SetNextWindowPos({ 1000.0f + 20.0f, 20.0f });
	ImGui::SetNextWindowSize({ m_Impl->m_WindowSize.x + 20.0f, m_Impl->m_WindowSize.y + 50.0f });
	auto winFlags = ImGuiWindowFlags_NoScrollbar;

	ImGui::PushFont(m_Impl->m_FontManager->GetFont(app::FontManager::FontType::Regular));
	ImGui::Begin("Video Viewport", &isActive, winFlags);
	{
		auto winSize = ImGui::GetWindowSize();	
		//LOG_DEBUG << "Window Size: " << winSize.x << " x " << winSize.y;
		if (winSize.x != m_Impl->m_WindowSize.x + 20.0f || winSize.y != m_Impl->m_WindowSize.y + 50.0f)
		{
			/*m_Impl->m_WindowSize.y = winSize.y + 50.0f;
			m_Impl->m_WindowSize.x = winSize.y * m_Impl->m_DisplayAspectRatio + 20.0f;
			ImGui::SetWindowSize(m_Impl->m_WindowSize);*/
		}
		if (m_Impl->m_DisplayTextureWidth != 0 && m_Impl->m_DisplayTextureHeight != 0)
		{
			ImGui::Image((void*)m_Impl->m_TextureHandle, m_Impl->m_WindowSize);
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

rxcpp::observer<uint2> fu::fusion::PlayerViewportView::FrameSizeFlowIn()
{
	return m_Impl->m_FrameSizeFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<float> PlayerViewportView::OnViewportWidthChanged()
{
	return m_Impl->m_OnViewportWidthChangedSubj.get_observable().as_dynamic();
}
rxcpp::observable<float> PlayerViewportView::OnViewportHeightChanged()
{
	return m_Impl->m_OnViewportHeightChangedSubj.get_observable().as_dynamic();
}
rxcpp::observable<GLuint> fu::fusion::PlayerViewportView::PixelBufferHandleFlowOut()
{
	return m_Impl->m_PixelBufferFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<float2> fu::fusion::PlayerViewportView::OnViewportSizeChanged()
{
	return m_Impl->m_OnViewportSizeChangedSubj.get_observable().as_dynamic();
}

}	///	!namespace fusion
}	///	!namespace fu