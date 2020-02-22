#include <Views/MainToolbarView.h>
#include <FontManager.h>
#include <imgui.h>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief MainToolbarView implementation
struct MainToolbarView::Impl
{	
	fman_ptr_t	m_FontManager;

	rxcpp::subjects::subject<void*> FileMenu_OpenProjectClickedSubj;
	rxcpp::subjects::subject<void*> FileMenu_SaveProjectAsClickedSubj;
	rxcpp::subjects::subject<void*> FileMenu_SaveProjectClickedSubj;
	rxcpp::subjects::subject<void*> FileMenu_OpenVideoFileClickedSubj;

	rxcpp::subjects::subject<void*> FiltersMenu_ClickedSubj;
	/// Construction
	Impl(fman_ptr_t fman)
		: m_FontManager(fman)
	{ }
};	///	!struct Impl
/// Construction
MainToolbarView::MainToolbarView(fman_ptr_t fman)
	: app::Widget("Main Toolbar"), m_Impl(spimpl::make_unique_impl<Impl>(fman))
{ }
///	Widget Rendering
void MainToolbarView::Render()
{
	if (!IsActive())
		return;

	ImGui::PushFont(m_Impl->m_FontManager->GetFont(app::FontManager::FontType::Regular));
	ImGui::BeginMainMenuBar();
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open Project"))
			{
				m_Impl->FileMenu_OpenProjectClickedSubj.get_subscriber().on_next(nullptr);
			}
			if (ImGui::MenuItem("Save Project as"))
			{
				m_Impl->FileMenu_SaveProjectAsClickedSubj.get_subscriber().on_next(nullptr);
			}
			if (ImGui::MenuItem("Save Project"))
			{
				m_Impl->FileMenu_SaveProjectClickedSubj.get_subscriber().on_next(nullptr);
			}
			if (ImGui::MenuItem("Open Video File"))
			{
				m_Impl->FileMenu_OpenVideoFileClickedSubj.get_subscriber().on_next(nullptr);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Filters"))
		{
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
	ImGui::PopFont();
}

rxcpp::observable<void*> fusion::MainToolbarView::OnFileMenu_OpenProjectClicked()
{
	return m_Impl->FileMenu_OpenProjectClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fusion::MainToolbarView::OnFileMenu_SaveProjectAsClicked()
{
	return m_Impl->FileMenu_SaveProjectAsClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fusion::MainToolbarView::OnFileMenu_SaveProjectClicked()
{
	return m_Impl->FileMenu_SaveProjectClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fusion::MainToolbarView::OnFileMenu_OpenVideoFileClicked()
{
	return m_Impl->FileMenu_OpenVideoFileClickedSubj.get_observable().as_dynamic();
}
}
}