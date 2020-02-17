#ifndef	__APPLICATION_PUBLIC_IMGUIWINDOW_H__
#define __APPLICATION_PUBLIC_IMGUIWINDOW_H__

#include <spimpl.h>
#include <string.h>

#include <rxcpp/rx.hpp>

struct GLFWwindow;

namespace app {
namespace input {
class InputManager;
}	///	!namespace input
class WindowFlags;
class Renderable;
///	\class ImGuiWindow
///	\brie an imgui top level window
class ImGuiWindow
{
public:
	using winflags_ptr_t = std::shared_ptr<WindowFlags>;
	/// construction
	ImGuiWindow(const std::string& windowTitle, winflags_ptr_t windowFlags, Renderable& renderable, input::InputManager& inputMan);
	/// initialize the window
	void Init();
	///	update the window
	void Update();
	///	destroy the window
	void Destroy();
private:
	void Render();

	friend void glfw_error_cb(int error, const char* description);
	friend void glfw_resize_cb(GLFWwindow*, int width, int height);
	friend void glfw_mouse_button_cb(GLFWwindow* window, int button, int action, int mods);
	friend void glfw_keyboard_cb(GLFWwindow* window, int key, int action, int mods);
	friend void glfw_mouse_position_cb(GLFWwindow* window, double posx, double posy);
	friend void glfw_scroll_cb(GLFWwindow* window, double xoffset, double yoffset);
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
	///
};	///	!class ImGuiWindow
}	///	!namespace app
#endif	///	!__APPLICATION_PUBLIC_IMGUIWINDOW_H__