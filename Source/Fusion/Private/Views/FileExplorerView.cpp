#include <Views/FileExplorerView.h>
#include <imgui.h>

namespace fusion {
struct FileExplorerView::Impl
{
	FileExplorerMode	m_Mode;
	drive_letter_array 	m_DriveLetters;
	dir_entry_array		m_CurrentDirEntries;
	dir_entry_array		m_CurrentDirHierarchy;
	unsigned int		m_DriveCount{ 0 };

	std::string m_CurrentDirectory;

	rxcpp::subjects::subject<std::string>		CurrentDirectoryFlowInSubj;
	rxcpp::subjects::subject<dir_entry_array>	CurrentDirEntriesFlowInSubj;
	rxcpp::subjects::subject<dir_entry_array>	CurrentDirHierarchyISubj;
	rxcpp::subjects::subject<unsigned int>		DriveCountFlowInSubj;
	rxcpp::subjects::subject<drive_letter_array> DriveLettersFlowInSubj;
};
/// Construction
FileExplorerView::FileExplorerView()
	: app::Widget("FileExplorer"), m_Impl(spimpl::make_unique_impl<Impl>())
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
	ImGui::Begin("File explorer");
	{
		
	}
	ImGui::End();
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
}	///	!namespace fusion