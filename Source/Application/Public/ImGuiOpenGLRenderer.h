#ifndef	__APPLICATION_PUBLIC_IMGUIOPENGLRENDERER_H__
#define __APPLICATION_PUBLIC_IMGUIOPENGLRENDERER_H__

#include <spimpl.h>

namespace app {
///	\class ImGuiOpenGLRenderer
///	\brief window renderer for imgui
class ImGuiOpenGLRenderer
{
public:
	/// Construction
	ImGuiOpenGLRenderer();
	/// \brief initialize
	void Init();
	///	\brief render a new frame
	void NewFrame();
	///	\brief render
	void Render();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class ImGuiOpenGLRenderer
}	///	!namespace app
#endif	///	!__APPLICATION_PUBLIC_IMGUIOPENGLRENDERER_H__