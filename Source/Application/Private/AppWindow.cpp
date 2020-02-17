#include <AppWindow.h>
#include <ImGuiWindow.h>

namespace app {
///	\struct Impl
///	\brief AppWindow implementation
struct AppWindow::Impl
{
	ImGuiWindow& m_Window;
	/// Construction
	Impl(ImGuiWindow& window)
		: m_Window(window)
	{ }
};	///	!struct Impl
/// Coinstruction
AppWindow::AppWindow(ImGuiWindow& window)
	: m_Impl(spimpl::make_unique_impl<Impl>(window))
{ }

void AppWindow::Init()
{
	m_Impl->m_Window.Init();
}

void AppWindow::Update()
{
	m_Impl->m_Window.Update();
}

void AppWindow::Destroy()
{
	m_Impl->m_Window.Destroy();
}
}	///	!namespace app