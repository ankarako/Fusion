#include <Views/PlayerViewportView.h>
#include <Core/Coordination.h>
#include <Buffer.h>
#include <FontManager.h>
#include <imgui.h>
#include <GL/gl3w.h>
#include <plog/Log.h>

// FIXME: delete
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

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
	ImVec2		m_WindowSize{ 0.0f, 0.0f };
	/// gl texture width
	int m_DisplayTextureWidth{ 0 };
	///	gl texture height
	int m_DisplayTextureHeight{ 0 };
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
	/// pixel buffer flow out event
	rxcpp::subjects::subject<GLuint>	m_PixelBufferFlowOutSubj;
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
	m_Impl->m_FrameWidthFlowInSubj.get_observable().observe_on(m_Impl->m_Coord->UICoordination())
		.with_latest_from(m_Impl->m_FrameHeightFlowinSubj.get_observable())
		.subscribe([this](auto wh) 
	{
		//m_Impl->m_DisplayTextureWidth = std::get<0>(wh);
		//m_Impl->m_DisplayTextureHeight = std::get<1>(wh);
		///// create a gl texture
		//glGenTextures(1, &m_Impl->m_DisplayTexture);
		///// bind the texture for setup
		//glBindTexture(GL_TEXTURE_2D, m_Impl->m_DisplayTexture);
		///// set min filter
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		///// for no input fill a buffer with zeros and copy this one to the gpu
		//BufferCPU<uchar4> blackBuf = CreateBufferCPU<uchar4>(m_Impl->m_DisplayTextureWidth * m_Impl->m_DisplayTextureHeight);
		//std::memset(blackBuf->Data(), 1, blackBuf->ByteSize());
		//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Impl->m_DisplayTextureWidth, m_Impl->m_DisplayTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)blackBuf->Data());
	});
	///====================
	/// frame flow in task
	///====================
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
		/// create pixel buffer for saving the texture
		glGenBuffers(1, &m_Impl->m_PixelBufferHandle);
		if (m_Impl->m_PixelBufferHandle == 0)
		{
			LOG_ERROR << "Failed to create pixel buffer for displaying video texture.";
		}
		/// bind the generated buffer to gl state
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_Impl->m_PixelBufferHandle);
		/// create the buffer's storage
		/// viewport buffer always 8UCRGBA
		glBufferData(GL_PIXEL_UNPACK_BUFFER, size.x * size.y * sizeof(unsigned char) * 4, nullptr, GL_STREAM_READ);
		/// unbind the buffer from gl state
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
	m_Impl->m_FrameFlowInSubj.get_observable()
		.subscribe([this](BufferCPU<uchar4>& frame) 
	{
		// FIXME: delete this
		cv::Mat mat = cv::Mat::zeros(m_Impl->m_DisplayTextureHeight, m_Impl->m_DisplayTextureWidth, CV_8UC4);
		std::memcpy(mat.data, frame->Data(), frame->ByteSize());
		cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGR);
		cv::imwrite("frame_in_render.png", mat);
		/// bind our gl texture to gl state
		glBindTexture(GL_TEXTURE_2D, m_Impl->m_TextureHandle);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Impl->m_DisplayTextureWidth, m_Impl->m_DisplayTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)frame->Data());
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

	auto winFlags = ImGuiWindowFlags_NoScrollbar;

	ImGui::PushFont(m_Impl->m_FontManager->GetFont(app::FontManager::FontType::Regular));
	ImGui::Begin("Video Viewport", &isActive, winFlags);
	{
		ImVec2 winSize = ImGui::GetWindowSize();
		if (winSize.x != m_Impl->m_WindowSize.x)
		{
			m_Impl->m_WindowSize.x = winSize.x;
			m_Impl->m_WindowSize.y = m_Impl->m_WindowSize.x / 2;
			m_Impl->m_OnViewportWidthChangedSubj.get_subscriber().on_next(m_Impl->m_WindowSize.x);
			m_Impl->m_OnViewportHeightChangedSubj.get_subscriber().on_next(m_Impl->m_WindowSize.y);
			ImGui::SetWindowSize(m_Impl->m_WindowSize);
		}

		if (winSize.y != m_Impl->m_WindowSize.y)
		{
			m_Impl->m_WindowSize.y = winSize.y;
			m_Impl->m_WindowSize.x = 2 * m_Impl->m_WindowSize.y;
			m_Impl->m_OnViewportHeightChangedSubj.get_subscriber().on_next(m_Impl->m_WindowSize.y);
			m_Impl->m_OnViewportWidthChangedSubj.get_subscriber().on_next(m_Impl->m_WindowSize.x);
			ImGui::SetWindowSize(m_Impl->m_WindowSize);
		}
		if (m_Impl->m_DisplayTextureWidth != 0 && m_Impl->m_DisplayTextureHeight != 0)
		{
			ImGui::Image((void*)(intptr_t)m_Impl->m_TextureHandle, m_Impl->m_WindowSize);
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
}	///	!namespace fusion
}	///	!namespace fu