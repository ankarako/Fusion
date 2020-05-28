#include <Views/ViewportView.h>
#include <imgui.h>
#include <GL/gl3w.h>
#include <plog/Log.h>
#include <Utils/Arcball.h>

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
	rxcpp::subjects::subject<mat_t>				m_RotationTransformFlowOutSubj;
	rxcpp::subjects::subject<trans_vec_t>		m_TranslationVectorFlowOutSubj;
	rxcpp::subjects::subject<void*>				m_LeftMouseStartDraggingFlowOutSubj;
	rxcpp::subjects::subject<void*>				m_RightMouseStartDraggingFlowOutSubj;
	rxcpp::subjects::subject<void*>				m_LeftMouseStopDraggingFlowOutSubj;
	rxcpp::subjects::subject<void*>				m_RightMouseStopDraggingFlowOutSubj;
	rxcpp::subjects::subject<mouse_pos_t>		m_LeftMouseButtonPosFlowOutSubj;
	rxcpp::subjects::subject<mouse_pos_t>		m_RightMouseButtonPosFlowOoutSubj;
	rxcpp::subjects::subject<int>				m_WheelEventFlowOutSubj;
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

	ImGuiIO& io = ImGui::GetIO();
	ImGui::Begin("Main Viewport", &isActive, flags);
	{
		/// Handle window resizing
		ImVec2 windowSize = ImGui::GetWindowSize();
		if (windowSize.x != m_Impl->m_ViewportSize[0] + m_Impl->m_ViewportSizeExpand.x || windowSize.y != m_Impl->m_ViewportSize[1] + m_Impl->m_ViewportSizeExpand.y)
		{
			m_Impl->m_ViewportSize = { (int)windowSize.x, (int)windowSize.x / 2 };
			m_Impl->m_ViewportSizeFlowOutSubj.get_subscriber().on_next(m_Impl->m_ViewportSize);
		}
		if (m_Impl->m_ViewportTextureHandle != 0)
		{
			ImGui::Image((void*)m_Impl->m_ViewportTextureHandle, { (float)m_Impl->m_ViewportSize[0], (float)m_Impl->m_ViewportSize[1] });
		}
		/// get mouse events
		if (ImGui::IsWindowFocused())
		{
			/// left mouse button
			if (io.MouseDown[0])
			{
				m_Impl->m_LeftMouseStartDraggingFlowOutSubj.get_subscriber().on_next(nullptr);
				ImVec2 pos = ImGui::GetMousePos();
				mouse_pos_t mpos = { (int)pos.x, (int)pos.y };
				m_Impl->m_LeftMouseButtonPosFlowOutSubj.get_subscriber().on_next(mpos);

				ImVec2 delta = ImGui::GetMouseDragDelta(0);
				if (delta.x != 0.0f || delta.y != 0.0f)
				{
					ImVec2 curMousePos = ImGui::GetMousePos();
					delta = ImVec2(delta.x /*/ m_Impl->m_ViewportSize[0]*/, delta.y /*/ m_Impl->m_ViewportSize[1]*/);
					mat_t mat;
					rt::Arcball::Rotate(curMousePos.x, curMousePos.y, curMousePos.x + delta.x, curMousePos.y + delta.y, 5.0f, mat);
					m_Impl->m_RotationTransformFlowOutSubj.get_subscriber().on_next(mat);
				}
			}
			if (io.MouseReleased[0])
			{
				m_Impl->m_LeftMouseStopDraggingFlowOutSubj.get_subscriber().on_next(nullptr);
			}
			/// right mouse button
			if (io.MouseDown[1])
			{
				m_Impl->m_RightMouseStartDraggingFlowOutSubj.get_subscriber().on_next(nullptr);
				ImVec2 pos = ImGui::GetMousePos();
				mouse_pos_t mpos = { (int)pos.x, (int)pos.y };
				m_Impl->m_RightMouseButtonPosFlowOoutSubj.get_subscriber().on_next(mpos);
				ImVec2 delta = ImGui::GetMouseDragDelta(1);
				if (delta.x != 0.0f || delta.y != 0.0f)
				{
					ImVec2 curMousePos = ImGui::GetMousePos();
					delta = ImVec2(delta.x / m_Impl->m_ViewportSize[0], delta.y / m_Impl->m_ViewportSize[1]);
					trans_vec_t vec;
					rt::Arcball::Translate(curMousePos.x, curMousePos.y, curMousePos.x + delta.x, curMousePos.y + delta.y, vec);
					m_Impl->m_TranslationVectorFlowOutSubj.get_subscriber().on_next(vec);
				}
			}
			if (io.MouseReleased[1])
			{
				m_Impl->m_RightMouseStopDraggingFlowOutSubj.get_subscriber().on_next(nullptr);
			}
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
rxcpp::observable<ViewportView::mat_t> ViewportView::RotationTransformFlowOut()
{
	return m_Impl->m_RotationTransformFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<ViewportView::trans_vec_t> ViewportView::TranslationVectorFlowOut()
{
	return m_Impl->m_TranslationVectorFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<void*> ViewportView::LeftMouseButtonDownFlowOut()
{
	return m_Impl->m_LeftMouseStartDraggingFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<void*> ViewportView::LeftMouseButtonReleasedFlowOut()
{
	return m_Impl->m_LeftMouseStopDraggingFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<void*> ViewportView::RightMouseButtonDownFlowOut()
{
	return m_Impl->m_RightMouseStartDraggingFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<void*> ViewportView::RightMouseButtonReleasedFlowOut()
{
	return m_Impl->m_RightMouseStopDraggingFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<ViewportView::mouse_pos_t> ViewportView::LeftMouseButtonPosFlowOut()
{
	return m_Impl->m_LeftMouseButtonPosFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<ViewportView::mouse_pos_t> ViewportView::RightMouseButtonPosFlowOut()
{
	return m_Impl->m_RightMouseButtonPosFlowOoutSubj.get_observable().as_dynamic();
}
}	///	!namespace mvt
}	///	!namespace fu