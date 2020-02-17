#ifndef __APPLICATION_PUBLIC_APPWINDOW_H__
#define __APPLICATION_PUBLIC_APPWINDOW_H__

#include <Initializable.h>
#include <Updateable.h>
#include <Destroyable.h>

#include <spimpl.h>

namespace app {
///
class ImGuiWindow;
///	\class AppWindow
///	\brief an application window
class AppWindow : public Initializable, public Updateable, public Destroyable
{
public:
	///	Cosntruction
	AppWindow(ImGuiWindow& window);
	///	
	virtual void Init() override;
	virtual void Update() override;
	virtual void Destroy() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class AppWindow
}	///	!namespace app
#endif	///	!__APPLICATION_PUBLIC_APPWINDOW_H__