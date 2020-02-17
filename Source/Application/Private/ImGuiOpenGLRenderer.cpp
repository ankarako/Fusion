#include <ImGuiOpenGLRenderer.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <GL/gl3w.h>
#include <GL/GL.h>

namespace app {
///	\struct Impl
///	\brief ImGuiOpenGLRenderer Implementation
struct ImGuiOpenGLRenderer::Impl
{
	bool m_Initialized{ false };
};	///	!struct Impl
/// Construction
ImGuiOpenGLRenderer::ImGuiOpenGLRenderer()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
///	\breif initialize
void ImGuiOpenGLRenderer::Init()
{
	const char* glsl_version = "#version 410";
	ImGui_ImplOpenGL3_Init(glsl_version);
}
/// \brief render on new frame (swap the frame buffer)
void ImGuiOpenGLRenderer::NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
}
///	\brief render draw data
void ImGuiOpenGLRenderer::Render()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
}	///	!namespace app