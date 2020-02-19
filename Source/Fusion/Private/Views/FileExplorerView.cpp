#include <Views/FileExplorerView.h>
#include <imgui.h>
#include <FontManager.h>

namespace fusion {
struct FileExplorerView::Impl
{
	fman_ptr_t			m_FontManager;

	FileExplorerMode	m_Mode;
	drive_letter_array 	m_DriveLetters;
	dir_entry_array		m_CurrentDirEntries;
	dir_entry_array		m_CurrentDirHierarchy;
	unsigned int		m_DriveCount{ 0 };

	std::string m_CurrentDirectory;
	std::string m_SelectedFile;


	rxcpp::subjects::subject<std::string>		CurrentDirectoryFlowInSubj;
	rxcpp::subjects::subject<dir_entry_array>	CurrentDirEntriesFlowInSubj;
	rxcpp::subjects::subject<dir_entry_array>	CurrentDirHierarchyISubj;
	rxcpp::subjects::subject<unsigned int>		DriveCountFlowInSubj;
	rxcpp::subjects::subject<drive_letter_array> DriveLettersFlowInSubj;
	rxcpp::subjects::subject<std::string>		OpenProjectFileSubj;
	rxcpp::subjects::subject<std::string>		SaveProjectFileSubj;
	rxcpp::subjects::subject<std::string>		OpenVideoFileSubj;

	Impl(fman_ptr_t fman)
		: m_FontManager(fman)
	{ }
};
/// Construction
FileExplorerView::FileExplorerView(fman_ptr_t fman)
	: app::Widget("FileExplorer"), m_Impl(spimpl::make_unique_impl<Impl>(fman))
{ }

void FileExplorerView::Init()
{
	m_Impl->CurrentDirectoryFlowInSubj.get_observable().subscribe(
		[this](auto name) 
	{
		m_Impl->m_CurrentDirectory = name;
	});

	m_Impl->CurrentDirEntriesFlowInSubj.get_observable().subscribe(
		[this](auto dentries) 
	{
		m_Impl->m_CurrentDirEntries = dentries;
	});

	m_Impl->CurrentDirHierarchyISubj.get_observable().subscribe(
		[this](auto hier) 
	{
		m_Impl->m_CurrentDirHierarchy = hier;
	});

	m_Impl->DriveCountFlowInSubj.get_observable().subscribe(
		[this](auto count) 
	{
		m_Impl->m_DriveCount = count;
	});
}

void FileExplorerView::Render() 
{
	ImGui::PushFont(m_Impl->m_FontManager->GetFont(app::FontManager::FontType::Regular));
	ImGui::Begin("File explorer");
	{
		/// display current directory
		ImGui::Text(m_Impl->m_CurrentDirectory.c_str());
		ImGui::Separator();
		/// Render the directory hierarchy
		for (int dh = 0; dh < m_Impl->m_CurrentDirHierarchy->size(); dh++)
		{
			std::string label = m_Impl->m_CurrentDirEntries->at(dh).Name + "##hier";
			if (ImGui::Button(label.c_str()))
			{

			}
			ImGui::SameLine();
		}
	}
	ImGui::End();
	ImGui::PopFont();
}

void FileExplorerView::SetMode(FileExplorerMode mode)
{
	m_Impl->m_Mode = mode;
}

rxcpp::observer<std::string> fusion::FileExplorerView::CurrentDirectoryFlowIn()
{
	return m_Impl->CurrentDirectoryFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<FileExplorerView::dir_entry_array> fusion::FileExplorerView::CurrentDirEntriesFlowIn()
{
	return m_Impl->CurrentDirEntriesFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<FileExplorerView::dir_entry_array> fusion::FileExplorerView::CurrentDirHierarchyIn()
{
	return m_Impl->CurrentDirHierarchyISubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<unsigned int> fusion::FileExplorerView::DriveCountFlowIn()
{
	return m_Impl->DriveCountFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<FileExplorerView::drive_letter_array> fusion::FileExplorerView::DriveLettersFlowIn()
{
	return m_Impl->DriveLettersFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<std::string> fusion::FileExplorerView::OpenProjectFileFlowOut()
{
	return m_Impl->OpenProjectFileSubj.get_observable().as_dynamic();
}

rxcpp::observable<std::string> fusion::FileExplorerView::SaveProjectFileFlowOut()
{
	return m_Impl->SaveProjectFileSubj.get_observable().as_dynamic();
}

rxcpp::observable<std::string> fusion::FileExplorerView::OpenVideoFileFlowOut()
{
	return m_Impl->OpenVideoFileSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion