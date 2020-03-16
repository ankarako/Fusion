#include <Models/FileExplorerModel.h>
#include <fstream>
#include <sstream>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#if defined(_WIN32) || defined(_WIN64)
	#include <Windows.h>
	#include <tchar.h>
	//#include <wchar.h>
#endif
namespace fu {
namespace fusion {
///	\struct Impl
///	\brief File explorer model implmentation
struct FileExplorerModel::Impl
{
	static constexpr unsigned int k_DriveBufferLength = 1024;

	drive_letter_array	m_DriveLetters;
	dir_entry_array		m_CurrentDirEntries;
	dir_entry_array		m_CurrentDirHierarchy;
	std::string			m_CurrentDirectory;
	std::string			m_CurrentDrive;

	rxcpp::subjects::subject<std::string>			CurrentDirectoryFlowOutSubj;
	rxcpp::subjects::subject<dir_entry_array>		CurrentDirEntriesFlowOutSubj;
	rxcpp::subjects::subject<dir_entry_array>		CurrentDirHierarchyFlowOutSubj;
	rxcpp::subjects::subject<unsigned int>			DriveCountFlowOutSubj;
	rxcpp::subjects::subject<drive_letter_array>	DriveLettersFlowOutSubj;
	rxcpp::subjects::subject<std::string>			CurrentSelectedDriveFlowOutSubj;
	/// Contruction
	Impl()
	{
		m_DriveLetters =
			std::make_shared<std::vector<std::string>>();
		m_CurrentDirEntries =
			std::make_shared<std::vector<DirEntry>>();
		m_CurrentDirHierarchy =
			std::make_shared<std::vector<DirEntry>>();
	}
	///	\brief get system drives
	void GetSystemDrives()
	{
#if defined(_WIN32) || defined(_WIN64)
		DWORD drive_buf_len = k_DriveBufferLength;
		TCHAR DriveBuffer[k_DriveBufferLength];
		memset(DriveBuffer, 0, k_DriveBufferLength * sizeof(TCHAR));
		DWORD res = GetLogicalDriveStrings(drive_buf_len, DriveBuffer);
		TCHAR* pcd = DriveBuffer;
		while (*pcd)
		{
			std::string d = std::string(1, char(*pcd)) + ":";
			m_DriveLetters->emplace_back(d);
			pcd = &pcd[_tcslen(pcd) + 1];
		}
		DriveLettersFlowOutSubj.get_subscriber().on_next(m_DriveLetters);
		DriveCountFlowOutSubj.get_subscriber().on_next(m_DriveLetters->size());
#endif
	}
	/// \brief get the current directory's entries
	void GetCurrentDirectoryEntries()
	{
		m_CurrentDirEntries->clear();
		for (auto path : std::experimental::filesystem::directory_iterator(m_CurrentDirectory))
		{
			m_CurrentDirEntries->emplace_back(ConvertToEntry(path.path().generic_string()));
		}
		CurrentDirectoryFlowOutSubj.get_subscriber().on_next(m_CurrentDirectory);
		CurrentDirEntriesFlowOutSubj.get_subscriber().on_next(m_CurrentDirEntries);
	}
	///	\brief convert a string to a directory entry
	DirEntry ConvertToEntry(const std::string& entry)
	{
		std::experimental::filesystem::path p(entry);
		DirEntry::Type type;
		if (std::experimental::filesystem::is_directory(p))
		{
			type = DirEntry::Type::Folder;
		}
		else
		{
			type = DirEntry::Type::File;
		}

		DirEntry dentry{
			p.filename().generic_string(),
			p.generic_string(),
			type
		};
		return dentry;
	}
	/// \brief get current directory hierarchy
	void GetCurrentDirHierarchy()
	{
		m_CurrentDirHierarchy->clear();
		m_CurrentDirHierarchy->emplace_back(ConvertToEntry(m_CurrentDirectory));
		std::experimental::filesystem::path tmp = m_CurrentDirectory;
		while (tmp.has_parent_path())
		{
			std::string parentName = tmp.parent_path().filename().generic_string();
			if (parentName.find(':') != std::string::npos)
			{
				for (auto drive : *m_DriveLetters.get())
				{
					if (drive == parentName)
					{
						m_CurrentDrive = drive;
						CurrentSelectedDriveFlowOutSubj.get_subscriber().on_next(m_CurrentDrive);
					}
				}
				tmp = tmp.parent_path();
				continue;
			}
			m_CurrentDirHierarchy->emplace_back(ConvertToEntry(tmp.parent_path().generic_string()));
			tmp = tmp.parent_path();
		}
		std::reverse(m_CurrentDirHierarchy->begin(), m_CurrentDirHierarchy->end());
		CurrentDirHierarchyFlowOutSubj.get_subscriber().on_next(m_CurrentDirHierarchy);
	}
};	///	!struct FileExplorerModel
/// Construction
///	\brief default constructor
///	allocates new implementation struct
FileExplorerModel::FileExplorerModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
///	\brief Initialize the file explorer 
///	loads the ini file
void FileExplorerModel::Init() noexcept
{
	std::ifstream in_ini_fs("fileexplorer.ini");
	if (!in_ini_fs.good())
	{
		std::ofstream out_ini_fs("fileexplorer.ini");
		m_Impl->m_CurrentDirectory = std::experimental::filesystem::current_path().generic_string();
		out_ini_fs << m_Impl->m_CurrentDirectory;
		out_ini_fs.close();
		in_ini_fs = std::ifstream("fileexplorer.ini");
	}
	if (in_ini_fs.good())
	{
		std::string curdir;
		while (std::getline(in_ini_fs, curdir))
		{
			m_Impl->m_CurrentDirectory = curdir;
			m_Impl->CurrentDirectoryFlowOutSubj.get_subscriber().on_next(m_Impl->m_CurrentDirectory);
		}
	}
	in_ini_fs.close();
	m_Impl->GetSystemDrives();
	m_Impl->GetCurrentDirectoryEntries();
	m_Impl->GetCurrentDirHierarchy();
}
///	\brief destroy the file explorer
void FileExplorerModel::Destroy() noexcept
{
	std::ofstream out_ini_fs("fileexplorer.ini");
	out_ini_fs << m_Impl->m_CurrentDirectory;
	out_ini_fs.close();
}
rxcpp::observable<std::string> FileExplorerModel::CurrentSelectedDriveFlowOut()
{
	return m_Impl->CurrentSelectedDriveFlowOutSubj.get_observable().as_dynamic();
}
///	\brief move one directory up
void FileExplorerModel::MoveUp() noexcept
{
	m_Impl->m_CurrentDirectory =
		std::experimental::filesystem::path(m_Impl->m_CurrentDirectory).parent_path().generic_string();
	m_Impl->GetCurrentDirectoryEntries();
	m_Impl->GetCurrentDirHierarchy();
}
///	\brief mnove into the incoming directory
void FileExplorerModel::MoveInto(const std::string& path) noexcept
{
	m_Impl->m_CurrentDirectory = path;
	m_Impl->GetCurrentDirectoryEntries();
	m_Impl->GetCurrentDirHierarchy();
}
void fusion::FileExplorerModel::SetCurrentDrive(const std::string& letter)
{
	m_Impl->m_CurrentDrive = letter;
	m_Impl->m_CurrentDirectory = letter;
	m_Impl->GetCurrentDirectoryEntries();
	m_Impl->GetCurrentDirHierarchy();
}

///	\brief get the current directory
///	\return the current directory
rxcpp::observable<std::string> fusion::FileExplorerModel::CurrentDirectoryFlowOut()
{
	return m_Impl->CurrentDirectoryFlowOutSubj.get_observable().as_dynamic();
}
///	\brief get the directory entries of the current directory
///	\return a vector with the direcoru entries in the current directory
rxcpp::observable<FileExplorerModel::dir_entry_array> fusion::FileExplorerModel::CurrentDirEntriesFlowOut()
{
	return m_Impl->CurrentDirEntriesFlowOutSubj.get_observable().as_dynamic();
}
///	\brief get the number of hardware drives in the system
///	\return the number of disk drives on the system
rxcpp::observable<unsigned int> fusion::FileExplorerModel::DriveCountFlowOut()
{
	return m_Impl->DriveCountFlowOutSubj.get_observable().as_dynamic();
}
///	\brief Get the hardware drive letters
///	\return a vector with the harware letters
rxcpp::observable<FileExplorerModel::drive_letter_array> fusion::FileExplorerModel::DriveLettersFlowOut()
{
	return m_Impl->DriveLettersFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<FileExplorerModel::dir_entry_array> fusion::FileExplorerModel::CurrentDirHierarchyFlowOut()
{
	return m_Impl->CurrentDirHierarchyFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namesoace fu