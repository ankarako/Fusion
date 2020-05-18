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
	rxcpp::subjects::behavior<viewport_size_t> 	m_ViewportSizeFlowOutSubj;
	rxcpp::subjects::subject<void*> 			m_OnCloseButtonClickedSubj;
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

}
/// \brief window render
void ViewportView::Render()
{
	if (!IsActive())
		return;
	
	ImGui::SetNextWindowSize({ (float)m_Impl->m_ViewportSize[0] , (float)m_Impl->m_ViewportSize[1] });
	ImGui::Begin("Main Viewport");
	{
		/// Handle window resizing
		ImVec2 windowSize = ImGui::GetWindowSize();
		if (windowSize.x != m_Impl->m_ViewportSize[0] || windowSize.y != m_Impl->m_ViewportSize[1])
		{
			m_Impl->m_ViewportSize = { (int)windowSize.x, (int)windowSize.y };
			LOG_DEBUG << "Window size changed";
		}
		//LOG_DEBUG << m_Impl->m_ViewportSize[0] << " x " << m_Impl->m_ViewportSize[1];
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
}	///	!namespace mvt
}	///	!namespace fu