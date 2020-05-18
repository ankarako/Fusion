#include <Views/MainToolbarView.h>
#include <FontManager.h>
#include <imgui.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief MainToolbarView implementation
struct MainToolbarView::Impl
{
	fman_ptr_t m_FontManager;
	rxcpp::subjects::subject<void*> m_FileMenu_ImportPerformanceClickedSubj;
	/// Construction
	///	\brief default constructor
	Impl(fman_ptr_t fman) 
		: m_FontManager(fman)
	{ }
};	///	!struct Impl
/// Construction
///	\brief default constructor
MainToolbarView::MainToolbarView(fman_ptr_t fman)
	: app::Widget("Main Toolbar"), m_Impl(spimpl::make_unique_impl<Impl>(fman))
{ }
///	\brief render the main toolbar
void MainToolbarView::Render()
{
	if (!IsActive())
		return;
	
	ImGui::PushFont(m_Impl->m_FontManager->GetFont(app::FontManager::FontType::Regular));
	ImGui::BeginMainMenuBar();
	{
		///===========
		/// File Menu
		///===========
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Import performance"))
			{
				m_Impl->m_FileMenu_ImportPerformanceClickedSubj.get_subscriber().on_next(nullptr);
			}
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
	ImGui::PopFont();
}

rxcpp::observable<void*> MainToolbarView::OnFileMenu_ImportPerformanceClicked()
{
	return m_Impl->m_FileMenu_ImportPerformanceClickedSubj.get_observable().as_dynamic();
}
}	///	!namespace mvt
}	///	!namespace fu