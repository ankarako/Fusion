#include <Views/FileExplorerView.h>
#include <imgui.h>
#include <FontManager.h>
#include <FontDef.h>

namespace fusion {
struct FileExplorerView::Impl
{
	fman_ptr_t			m_FontManager;

	FileExplorerMode	m_Mode;
	drive_letter_array 	m_DriveLetters;
	dir_entry_array		m_CurrentDirEntries;
	dir_entry_array		m_CurrentDirHierarchy;
	DirEntry			m_SelectedEntry;
	unsigned int		m_DriveCount{ 0 };
	std::string			m_CurrentDirectory;
	std::string			m_SelectedFile;

	static constexpr unsigned int	k_InputBufferSize = 512;

	std::string			m_CurrentSelectedDrive;
	ImVec2				m_ContentWindowSize{ -5, -50 };
	char				m_InputTextBuffer[k_InputBufferSize];
	

	///	inputs
	rxcpp::subjects::subject<std::string>			CurrentDirectoryFlowInSubj;
	rxcpp::subjects::subject<dir_entry_array>		CurrentDirEntriesFlowInSubj;
	rxcpp::subjects::subject<dir_entry_array>		CurrentDirHierarchyInSubj;
	rxcpp::subjects::subject<unsigned int>			DriveCountFlowInSubj;
	rxcpp::subjects::subject<drive_letter_array>	DriveLettersFlowInSubj;
	rxcpp::subjects::subject<std::string>			CurrentSelectedDriveFlowInSubj;
	/// outputs
	rxcpp::subjects::subject<std::string>		OpenProjectFileSubj;
	rxcpp::subjects::subject<std::string>		SaveProjectFileSubj;
	rxcpp::subjects::subject<std::string>		OpenVideoFileSubj;
	rxcpp::subjects::subject<void*>				OnMoveUpButtonClickedSubj;
	rxcpp::subjects::subject<std::string>		OnMoveIntoClickedSubj;

	Impl(fman_ptr_t fman)
		: m_FontManager(fman)
	{ 
		memset(&m_InputTextBuffer[0], 0, k_InputBufferSize * sizeof(char));
	}
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

	m_Impl->CurrentDirHierarchyInSubj.get_observable().subscribe(
		[this](auto hier) 
	{
		m_Impl->m_CurrentDirHierarchy = hier;
	});

	m_Impl->DriveCountFlowInSubj.get_observable().subscribe(
		[this](auto count) 
	{
		m_Impl->m_DriveCount = count;
	});

	m_Impl->DriveLettersFlowInSubj.get_observable().subscribe(
		[this](auto drive_letters) 
	{
		m_Impl->m_DriveLetters = drive_letters;
	});

	m_Impl->CurrentSelectedDriveFlowInSubj.get_observable().subscribe(
		[this](auto driveletter) 
	{
		m_Impl->m_CurrentSelectedDrive = driveletter;
	});
}

void FileExplorerView::Render() 
{
	bool selected = false;
	ImGui::PushFont(m_Impl->m_FontManager->GetFont(app::FontManager::FontType::Regular));
	ImGui::Begin("File explorer");
	{
		/// display current directory
		/*ImGui::Text(m_Impl->m_CurrentDirectory.c_str());
		ImGui::Separator();*/
		if (ImGui::Button(ICON_MD_ARROW_UPWARD "##moveup"))
		{
			if (m_Impl->m_CurrentDirHierarchy->size() > 1)
			{
				m_Impl->OnMoveUpButtonClickedSubj.get_subscriber().on_next(nullptr);
			}
		}
		ImGui::SameLine();
		/// Render the directory hierarchy
		ImGui::PushItemWidth(60);
		if (ImGui::BeginCombo("##drivecombo", m_Impl->m_CurrentSelectedDrive.c_str()))
		{
			for (int dr = 0; dr < m_Impl->m_CurrentDirEntries->size(); dr++)
			{
				//bool currentSelectedDrive = m_Impl->m_CurrentSelectedDrive == m_Impl->m_DriveLetters->at(dr);
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		for (int dh = 0; dh < m_Impl->m_CurrentDirHierarchy->size(); dh++)
		{
			ImGui::SameLine();
			std::string label = m_Impl->m_CurrentDirHierarchy->at(dh).Name + "##hier";
			if (ImGui::Button(label.c_str()))
			{
				m_Impl->OnMoveIntoClickedSubj.get_subscriber().on_next(m_Impl->m_CurrentDirHierarchy->at(dh).AbsPath);
			}
		}
		ImGui::Separator();
		ImGui::BeginChild("ScrollingRegion##fexp", m_Impl->m_ContentWindowSize);
		{
			/// display current dir entries
			for (int de = 0; de < m_Impl->m_CurrentDirEntries->size(); de++)
			{
				if (m_Impl->m_CurrentDirEntries->at(de).EntryType == DirEntry::Type::Folder)
				{
					std::string label = std::string(ICON_MD_FOLDER) + m_Impl->m_CurrentDirEntries->at(de).Name;
					if (ImGui::Selectable(label.c_str(), &selected))
					{

					}
				}
			}
			for (int de = 0; de < m_Impl->m_CurrentDirEntries->size(); de++)
			{
				if (m_Impl->m_CurrentDirEntries->at(de).EntryType == DirEntry::Type::File)
				{
					std::string label = std::string(ICON_MD_DESCRIPTION) + m_Impl->m_CurrentDirEntries->at(de).Name;
					if (ImGui::Selectable(label.c_str(), &selected))
					{

					}
				}
			}
		}
		ImGui::EndChild();
		ImGui::InputText(": Filename", m_Impl->m_InputTextBuffer, m_Impl->k_InputBufferSize);
		ImGui::SameLine();
		if (m_Impl->m_Mode == FileExplorerMode::OpenProject)
		{
			if (ImGui::Button("Open"))
			{

			}
		}
		else if (m_Impl->m_Mode == FileExplorerMode::OpenVideoFile)
		{
			if (ImGui::Button("Open"))
			{

			}
		}
		else if (m_Impl->m_Mode == FileExplorerMode::SaveProject)
		{
			if (ImGui::Button("Save"))
			{

			}
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
	return m_Impl->CurrentDirHierarchyInSubj.get_subscriber().get_observer().as_dynamic();
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

rxcpp::observable<void*> fusion::FileExplorerView::OnMoveUpButtonClicked()
{
	return m_Impl->OnMoveUpButtonClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<std::string> fusion::FileExplorerView::OnMoveIntoDirectory()
{
	return m_Impl->OnMoveIntoClickedSubj.get_observable().as_dynamic();
}

rxcpp::observer<std::string> fusion::FileExplorerView::CurrentSelectedDriveFlowIn()
{
	return m_Impl->CurrentSelectedDriveFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
}	///	!namespace fusion