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
	rxcpp::subjects::subject<void*> FileMenu_Open3DFileClickedSubj;
	rxcpp::subjects::subject<void*> FileMenu_OpenPerfcapFileClickedSubj;

	rxcpp::subjects::subject<void*> FiltersMenu_ClickedSubj;
	rxcpp::subjects::subject<void*> FiltersMenu_EstimateDepthClickedSubj;
	rxcpp::subjects::subject<void*> FiltersMenu_EstimateSurfNormalClickedSubj;
	rxcpp::subjects::subject<void*> FiltersMenu_EstimateIlluminationClickedSubj;


	rxcpp::subjects::subject<void*> WindowsMenu_RayTracingControlClickedSubj;
	rxcpp::subjects::subject<void*> WindowsMenu_NormalsResultClickedSubj;
	rxcpp::subjects::subject<void*> WindowsMenu_IlluminationResultClickedSubj;

	rxcpp::subjects::subject<void*> Omniconnect_ShowVideoListClickedSubj;
	rxcpp::subjects::subject<void*> Omniconnect_UploadMentorLayerClickedSubj;
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
			if (ImGui::MenuItem("Import Video File"))
			{
				m_Impl->FileMenu_OpenVideoFileClickedSubj.get_subscriber().on_next(nullptr);
			}
			if (ImGui::MenuItem("Import 3D File"))
			{
				m_Impl->FileMenu_Open3DFileClickedSubj.get_subscriber().on_next(nullptr);
			}
			if (ImGui::MenuItem("Import Perfcap File"))
			{
				m_Impl->FileMenu_OpenPerfcapFileClickedSubj.get_subscriber().on_next(nullptr);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Filters"))
		{
			if (ImGui::MenuItem("Depth Estimation"))
			{
				m_Impl->FiltersMenu_EstimateDepthClickedSubj.get_subscriber().on_next(nullptr);
			}
			if (ImGui::MenuItem("Surface Normal Estimation"))
			{
				m_Impl->FiltersMenu_EstimateSurfNormalClickedSubj.get_subscriber().on_next(nullptr);
			}
			if (ImGui::MenuItem("Illumination Estimation"))
			{
				m_Impl->FiltersMenu_EstimateIlluminationClickedSubj.get_subscriber().on_next(nullptr);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			if (ImGui::MenuItem("3D Viewport controls"))
			{
				m_Impl->WindowsMenu_RayTracingControlClickedSubj.get_subscriber().on_next(nullptr);
			}
			if (ImGui::MenuItem("Surface Normal Estimation Result"))
			{
				m_Impl->WindowsMenu_NormalsResultClickedSubj.get_subscriber().on_next(nullptr);
			}
			if (ImGui::MenuItem("Illumination Estimation Result"))
			{
				m_Impl->WindowsMenu_IlluminationResultClickedSubj.get_subscriber().on_next(nullptr);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Omniconnect"))
		{
			if (ImGui::MenuItem("Get Available Media"))
			{
				m_Impl->Omniconnect_ShowVideoListClickedSubj.get_subscriber().on_next(nullptr);
			}
			if (ImGui::MenuItem("Upload Mentor Layer"))
			{
				m_Impl->Omniconnect_UploadMentorLayerClickedSubj.get_subscriber().on_next(nullptr);
			}
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
rxcpp::observable<void*> fusion::MainToolbarView::OnFileMenu_Open3DFileClicked()
{
	return m_Impl->FileMenu_Open3DFileClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fu::fusion::MainToolbarView::OnFiltersMenu_EstimateDepthClicked()
{
	return m_Impl->FiltersMenu_EstimateDepthClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> MainToolbarView::OnWindowsMenu_RayTracingControlClicked()
{
	return m_Impl->WindowsMenu_RayTracingControlClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fu::fusion::MainToolbarView::OnFileMenu_OpenPerfcapFileClicked()
{
	return m_Impl->FileMenu_OpenPerfcapFileClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fu::fusion::MainToolbarView::OnFiltersMenu_EstimateNormalsClicked()
{
	return m_Impl->FiltersMenu_EstimateSurfNormalClickedSubj.get_observable().as_dynamic();
}
rxcpp::observable<void*> fu::fusion::MainToolbarView::OnWindowsMenu_NormalsResultClicked()
{
	return m_Impl->WindowsMenu_NormalsResultClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fu::fusion::MainToolbarView::OnFiltersMenu_EstimateIlluminationClicked()
{
	return m_Impl->FiltersMenu_EstimateIlluminationClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fu::fusion::MainToolbarView::OnWindowsMenu_IlluminationResultClicked()
{
	return m_Impl->WindowsMenu_IlluminationResultClickedSubj.get_observable().as_dynamic();
}
}	///	!namespce fusion
}	///	!namespace fu