#include <ImGuiWindow.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <Renderable.h>
#include <WindowFlags.h>
#include <ImGuiOpenGLRenderer.h>
#include <InputManager.h>

namespace app {
///	\struct Impl
///	\brief ImGuiWindow Implementation
struct ImGuiWindow::Impl
{
	winflags_ptr_t			m_WindowFlags;
	bool					m_Initialized{ false };
	bool					m_ShutDown{ false };
	bool 					m_Destroyed{ false };
	Renderable&				m_Renderable;
	input::InputManager&	m_InputManager;
	ImGuiOpenGLRenderer		m_GLRenderer;
	std::string				m_WindowTitle{ "Application Window" };

	GLFWwindow*				m_Window{ nullptr };

	Impl(const std::string& title, winflags_ptr_t winFlags, Renderable& renderable, input::InputManager& inman)
		: m_WindowFlags(winFlags)
		, m_Renderable(renderable)
		, m_InputManager(inman)
		, m_WindowTitle(title)
	{ }
};	///	!struct Impl

/// the only window handle
static ImGuiWindow* g_AppWindow;
/// Construction
ImGuiWindow::ImGuiWindow(const std::string& windowTitle, winflags_ptr_t windowFlags, Renderable& renderable, input::InputManager& inputMan)
	: m_Impl(spimpl::make_unique_impl<Impl>(windowTitle, windowFlags, renderable, inputMan))
{
	g_AppWindow = this;
}
///	\brief initialize the window
void ImGuiWindow::Init()
{
	/// setup the window
	glfwSetErrorCallback(glfw_error_cb);
	///
	if (!glfwInit())
	{
		m_Impl->m_WindowFlags->RequestShutDown();
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	m_Impl->m_Window = glfwCreateWindow(1920, 1080, m_Impl->m_WindowTitle.c_str(), NULL, NULL);
	if (m_Impl->m_Window == NULL)
	{
		m_Impl->m_WindowFlags->RequestShutDown();
		return;
	}
	glfwSetWindowSizeCallback(m_Impl->m_Window, glfw_resize_cb);
	glfwMakeContextCurrent(m_Impl->m_Window);
	glfwSwapInterval(1);	///	set v-sync

	int winW, winH;
	glfwGetWindowSize(m_Impl->m_Window, &winW, &winH);
	m_Impl->m_WindowFlags->SetWindowWidth(winW);
	m_Impl->m_WindowFlags->SetWindowHeight(winH);

	int frameBufW, frameBufH;
	glfwGetFramebufferSize(m_Impl->m_Window, &frameBufW, &frameBufH);
	m_Impl->m_WindowFlags->SetFrameBufferWidth(frameBufW);
	m_Impl->m_WindowFlags->SetFrameBufferHeight(frameBufH);

	/// Initialize open gl loader
	bool err = gl3wInit() != 0;
	if (err)
	{
		m_Impl->m_WindowFlags->RequestShutDown();
		return;
	}
	/// bind callbacks
	glfwSetCursorPosCallback(m_Impl->m_Window, glfw_mouse_position_cb);
	glfwSetMouseButtonCallback(m_Impl->m_Window, glfw_mouse_button_cb);
	glfwSetScrollCallback(m_Impl->m_Window, glfw_scroll_cb);

	/// Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplGlfw_InitForOpenGL(m_Impl->m_Window, true);
	m_Impl->m_GLRenderer.Init();

	/// setup style
	ImGui::StyleColorsDark();
	m_Impl->m_Initialized = true;
}

void ImGuiWindow::Render()
{
	ImVec4 clearColor(0.15f, 0.15f, 0.15f, 1.00f);
	/// start imgui frame
	m_Impl->m_GLRenderer.NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	int displayW, displayH;
	glfwGetFramebufferSize(m_Impl->m_Window, &displayW, &displayH);
	glViewport(0, 0, displayW, displayH);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	glClear(GL_COLOR_BUFFER_BIT);

	m_Impl->m_Renderable.Render();
	ImGui::Render();
	m_Impl->m_GLRenderer.Render();

	glfwSwapBuffers(m_Impl->m_Window);
}

void ImGuiWindow::Update()
{
	if (!m_Impl->m_Initialized || m_Impl->m_ShutDown || m_Impl->m_Destroyed)
		return;
	
	if (!glfwWindowShouldClose(m_Impl->m_Window))
	{
		glfwPollEvents();
		Render();
	}
	else
	{
		m_Impl->m_WindowFlags->RequestShutDown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		m_Impl->m_ShutDown = true;
	}
}

void ImGuiWindow::Destroy()
{
	if (!m_Impl->m_Destroyed)
	{
		glfwDestroyWindow(m_Impl->m_Window);
		glfwTerminate();
		m_Impl->m_Destroyed = true;
	}
}

/// glfw callbacks
static void glfw_error_cb(int error, const char* description)
{
	g_AppWindow->m_Impl->m_WindowFlags->RequestShutDown();
}

static void glfw_resize_cb(GLFWwindow* window, int width, int height)
{
	g_AppWindow->m_Impl->m_WindowFlags->SetWindowWidth(width);
	g_AppWindow->m_Impl->m_WindowFlags->SetWindowHeight(height);

	int frameBufferW;
	int frameBufferH;
	glfwGetFramebufferSize(window, &frameBufferW, &frameBufferH);

	g_AppWindow->m_Impl->m_WindowFlags->SetFrameBufferWidth(frameBufferW);
	g_AppWindow->m_Impl->m_WindowFlags->SetFrameBufferHeight(frameBufferW);

	g_AppWindow->Render();
}

void glfw_mouse_button_cb(GLFWwindow* window, int button, int action, int mods)
{
	input::MouseButton btn;
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		btn = input::MouseButton::Left;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		btn = input::MouseButton::Right;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		btn = input::MouseButton::Middle;
		break;
	default:
		break;
	}
	if (action == GLFW_PRESS)
	{
		g_AppWindow->m_Impl->m_InputManager.SetMouseButtonPressed(btn);
	}
	else if (action == GLFW_RELEASE)
	{
		g_AppWindow->m_Impl->m_InputManager.SetMouseButtonReleased(btn);
	}
}

void glfw_keyboard_cb(GLFWwindow* window, int key, int action, int mods)
{

}

void glfw_mouse_position_cb(GLFWwindow* window, double posx, double posy)
{
	input::MousePos pos{ posx, posy };
	g_AppWindow->m_Impl->m_InputManager.SetMousePosition(pos);
}

void glfw_scroll_cb(GLFWwindow* window, double xoffset, double yoffset)
{
	input::MouseScroll scroll{ xoffset, yoffset };
	g_AppWindow->m_Impl->m_InputManager.SetMouseScroll(scroll);
}
}	///	!namespace app