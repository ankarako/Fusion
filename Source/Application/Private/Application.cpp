#include <Application.h>

namespace app {
///	\struct impl
///	\brief Application implementation
struct Application::Impl
{
	winflags_ptr_t				m_WindowFlags;
	std::vector<init_ptr_t>		m_Initializables;
	std::vector<update_ptr_t>	m_Updateables;
	std::vector<destroy_ptr_t>	m_Destroyables;
	/// Construction
	Impl(
		winflags_ptr_t flags,
		std::vector<init_ptr_t> initializables, 
		std::vector<update_ptr_t> updateables,
		std::vector<destroy_ptr_t> destroyables)
		: m_WindowFlags(flags)
		, m_Initializables(initializables)
		, m_Updateables(updateables)
		, m_Destroyables(destroyables)
	{ }
};	///	!struct impl
/// Construction
Application::Application(
	winflags_ptr_t	flags,
	std::vector<init_ptr_t> initializables, 
	std::vector<update_ptr_t> updateables,
	std::vector<destroy_ptr_t> destroyables)
	: m_Impl(spimpl::make_unique_impl<Impl>(flags, initializables, updateables, destroyables))
{ }
/// \brief Run the application
void Application::Run()
{
	for (auto& initializable : m_Impl->m_Initializables)
	{
		initializable->Init();
	}
	while(!m_Impl->m_WindowFlags->PollShutDownFlag())
	{
		for (auto& updateable : m_Impl->m_Updateables)
		{
			updateable->Update();
		}
	}
	for (auto& destroyable : m_Impl->m_Destroyables)
	{
		destroyable->Destroy();
	}
}
}	///	!namespace app