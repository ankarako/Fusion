#include <Presenters/MainToolbarPresenter.h>
#include <Presenters/FileMenuPresenter.h>
#include <Presenters/FiltersMenuPresenter.h>
#include <Views/RayTracingControlView.h>
#include <Models/PerfcapPlayerModel.h>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief MainToolbarPresenter implementation
struct MainToolbarPresenter::Impl
{
	file_menu_ptr_t		m_FileMenu;
	filters_menu_ptr_t	m_FiltersMenu;

	Impl(
		file_menu_ptr_t		file_menu,
		filters_menu_ptr_t	filters_menu)
		: m_FileMenu(file_menu)
		, m_FiltersMenu(filters_menu)
	{ }
};	///	!struct Impl
///	Construction
MainToolbarPresenter::MainToolbarPresenter(
	file_menu_ptr_t		file_menu,
	filters_menu_ptr_t	filters_menu)
	: m_Impl(spimpl::make_unique_impl<Impl>(
		file_menu,
		filters_menu))
{ }
///	\brief Initialization
void MainToolbarPresenter::Init()
{

}
}	///	!namespace fusion
}	///	!namespace  fu