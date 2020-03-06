#ifndef	__APPLICATION_PUBLIC_APPLICATION_H__
#define __APPLICATION_PUBLIC_APPLICATION_H__

#include <Initializable.h>
#include <Updateable.h>
#include <Destroyable.h>
#include <WindowFlags.h>

#include <spimpl.h>

#include <vector>
#include <memory>

namespace fu {
namespace app {
///	\class app
///	\brief 	a simple application that holds application components
///	A simple initializable, updateable and destroyable holder
class Application
{
public:
	///	\typedef init_ptr_t
	///	\brief initializable shared pointer
	using init_ptr_t = std::shared_ptr<Initializable>;
	///	\typedef update_ptr_t
	///	\brief updateable shared pointer
	using update_ptr_t = std::shared_ptr<Updateable>;
	///	\typedef destroy_ptr_t
	///	\brief destroyable shared pointer
	using destroy_ptr_t = std::shared_ptr<Destroyable>;
	///	\typedef winflags_ptr_t
	using winflags_ptr_t = std::shared_ptr<WindowFlags>;
	/// Construction
	///	\brief construct from array of initializables, updateables and destroyables
	Application(
		winflags_ptr_t flags,
		std::vector<init_ptr_t> initializables,
		std::vector<update_ptr_t> updateables,
		std::vector<destroy_ptr_t> destroyables
	);
	///	\brief run the application
	void Run();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class Application
}	///	!namespace app
}	///	!namespace fu
#endif	///	!__APPLICATION_PUBLIC_APPLICATION_H__