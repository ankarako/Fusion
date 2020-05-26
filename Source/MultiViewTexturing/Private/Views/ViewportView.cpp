#include <Views/ViewportView.h>
#include <imgui.h>
#include <GL/gl3w.h>
#include <plog/Log.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief ViewportView Implementation
struct ViewportView::Impl
{
	GLuint										m_ViewportTextureHandle{ 0 };
	viewport_size_t 							m_ViewportSize{ 1000, 500 };
	ImVec2										m_ViewportSizeExpand{ 20.0f, 50.0f };
	rxcpp::subjects::behavior<viewport_size_t> 	m_ViewportSizeFlowOutSubj;
	rxcpp::subjects::subject<void*> 			m_OnCloseButtonClickedSubj;
	rxcpp::subjects::subject<BufferCPU<uchar4>>	m_FrameBufferFlowInSubj;
	/// Construction
	///	\brief default constructor
	Impl() 
		: m_ViewportSizeFlowOutSubj(rxcpp::subjects::behavior<viewport_size_t>(m_ViewportSize))
	{ }
};	///	!struct Impl
/// Construction
ViewportView::ViewportView()
	: app::Widget("Main Viewport"), m_Impl(spimpl::make_unique_impl<Impl>())
{ }
/// \brief Initialize the view
void ViewportView::Init()
{
	/// Initialize textures
	glGenTextures(1, &m_Impl->m_ViewportTextureHandle);
	if (m_Impl->m_ViewportTextureHandle == 0)
	{
		LOG_ERROR << "Failed to create texture handle for 3D viewport";
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Impl->m_ViewportTextureHandle);
	///
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	/// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
	///===================
	/// Frame flow in Task
	///===================
	m_Impl->m_FrameBufferFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const BufferCPU<uchar4>& frame) 
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Impl->m_ViewportTextureHandle);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Impl->m_ViewportSize[0], m_Impl->m_ViewportSize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, frame->Data());
		/// unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	});

}
/// \brief window render
void ViewportView::Render()
{
	bool isActive = this->IsActive();
	if (!isActive)
		return;
	
	ImGui::SetNextWindowSize({ (float)m_Impl->m_ViewportSize[0] + m_Impl->m_ViewportSizeExpand.x, (float)m_Impl->m_ViewportSize[1] + m_Impl->m_ViewportSizeExpand.y });
	ImGui::SetNextWindowPos({0, 20});
	int flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
	ImGui::Begin("Main Viewport", &isActive, flags);
	{
		/// Handle window resizing
		ImVec2 windowSize = ImGui::GetWindowSize();
		if (windowSize.x != m_Impl->m_ViewportSize[0] + m_Impl->m_ViewportSizeExpand.x || windowSize.y != m_Impl->m_ViewportSize[1] + m_Impl->m_ViewportSizeExpand.y)
		{
			m_Impl->m_ViewportSize = { (int)windowSize.x, (int)windowSize.y };
			m_Impl->m_ViewportSizeFlowOutSubj.get_subscriber().on_next(m_Impl->m_ViewportSize);
		}
		if (m_Impl->m_ViewportTextureHandle != 0)
		{
			ImGui::Image((void*)m_Impl->m_ViewportTextureHandle, { (float)m_Impl->m_ViewportSize[0], (float)m_Impl->m_ViewportSize[1] });
		}
	}
	ImGui::End();
}
rxcpp::observable<ViewportView::viewport_size_t> ViewportView::ViewportSizeFlowOut()
{
	return m_Impl->m_ViewportSizeFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<void*> ViewportView::OnCloseButtonClicked()
{
	return m_Impl->m_OnCloseButtonClickedSubj.get_observable().as_dynamic();
}
rxcpp::observer<BufferCPU<uchar4>> ViewportView::FrameBufferFlowIn()
{
	return m_Impl->m_FrameBufferFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
}	///	!namespace mvt
}	///	!namespace fu