#include <Views/RayTracingView.h>
#include <FontManager.h>
#include <ObsCoordination.h>
#include <imgui.h>
#include <GL/gl3w.h>
#include <plog/Log.h>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief 	Ray tracing view's implementation
struct RayTracingView::Impl
{
	fman_ptr_t	m_FMan;
	coord_ptr_t	m_Coord;
	ImVec2		m_ViewportSize{ 1280.0f, 720.0f };
	///	gl texture
	int m_DisplayTextureWidth{ 0 };
	int m_DisplayTextureHeight{ 0 };
	float m_DisplayAspectRatio{ 0.0f };
	/// gl texture to display on viewport
	GLuint m_TextureHandle{ 0 };
	/// inputs
	rxcpp::subjects::subject<BufferCPU<uchar4>>	m_FrameFlowInSubj;
	rxcpp::subjects::subject<float2>			m_OnViewportSizeChangedSubj;
	/// Construction
	Impl(fman_ptr_t fman, coord_ptr_t coord)
		: m_FMan(fman)
		, m_Coord(coord)
	{ }
};	///	!struct Impl
/// Cosntruction
RayTracingView::RayTracingView(fman_ptr_t fman, coord_ptr_t coord)
	: app::Widget("3D Viewport"), m_Impl(spimpl::make_unique_impl<Impl>(fman, coord))
{ }
/// \brief initialize the view
void RayTracingView::Init()
{
	/// initialize apsect ratio
	m_Impl->m_DisplayAspectRatio = static_cast<float>(m_Impl->m_ViewportSize.x) / static_cast<float>(m_Impl->m_ViewportSize.y);
	/// initialize fl texture
	glGenTextures(1, &m_Impl->m_TextureHandle);
	if (m_Impl->m_TextureHandle == 0)
	{
		LOG_ERROR << "Failed to crate texture handle for 3D viewport.";
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Impl->m_TextureHandle);
	/// set magnification and minimization filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	/// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
	/// notify about window size
	float2 size = make_float2(m_Impl->m_ViewportSize.x, m_Impl->m_ViewportSize.y);
	m_Impl->m_OnViewportSizeChangedSubj.get_subscriber().on_next(size);
	///====================
	/// frame flow in task
	///====================
	m_Impl->m_FrameFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const BufferCPU<uchar4>& frame) 
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Impl->m_TextureHandle);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Impl->m_ViewportSize.x, m_Impl->m_ViewportSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame->Data());
		/// unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	});
}
///	\brief render the widget
void RayTracingView::Render()
{
	bool isActive = this->IsActive();
	if (!isActive)
		return;

	ImGui::SetNextWindowSize(m_Impl->m_ViewportSize);
	auto winflags = ImGuiWindowFlags_NoScrollbar;

	ImGui::PushFont(m_Impl->m_FMan->GetFont(app::FontManager::FontType::Regular));
	ImGui::Begin("3D Viewport", &isActive, winflags);
	{
		
		auto winSize = ImGui::GetWindowSize();
		if (winSize.x != m_Impl->m_ViewportSize.x)
		{
			m_Impl->m_ViewportSize.x = winSize.y * m_Impl->m_DisplayAspectRatio;
			m_Impl->m_ViewportSize.y = winSize.y;
			ImGui::SetWindowSize(m_Impl->m_ViewportSize);
			float2 size = make_float2(m_Impl->m_ViewportSize.x, m_Impl->m_ViewportSize.y);
			m_Impl->m_OnViewportSizeChangedSubj.get_subscriber().on_next(size);
		}
		if (m_Impl->m_TextureHandle != 0)
		{
			ImGui::Image((void*)m_Impl->m_TextureHandle, m_Impl->m_ViewportSize);
		}
	}
	ImGui::End();
	ImGui::PopFont();
}
/// \brief fram input
rxcpp::observer<BufferCPU<uchar4>> fu::fusion::RayTracingView::FrameFlowIn()
{
	return m_Impl->m_FrameFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observable<float2> fu::fusion::RayTracingView::OnViewportSizeChanged()
{
	return m_Impl->m_OnViewportSizeChangedSubj.get_observable().as_dynamic();
}
}	///	!namesapce fusion
}	///	!namespace fu