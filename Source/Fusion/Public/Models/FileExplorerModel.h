#ifndef __FUSION_PUBLIC_MODELS_FILEEXPLORERMODEL_H__
#define __FUSION_PUBLIC_MODELS_FILEEXPLORERMODEL_H__

#include <Core/DirEntry.h>
#include <Initializable.h>
#include <Destroyable.h>
#include <spimpl.h>
#include <string>
#include <vector>
#include <rxcpp/rx.hpp>

namespace fusion {
///	\class FileExplorerModel
///	\brief File explorer logic
///	\note	the file explorer is somewhat autonomous
///	and saves its state (i.e. the last current directory 
///	to fileexplorer.ini) file in the binary directory
///	File loading is done in the Init() method
class FileExplorerModel : public app::Initializable, public app::Destroyable
{
public:
	using drive_letter_array	= std::shared_ptr<std::vector<std::string>>;
	using dir_entry_array		= std::shared_ptr<std::vector<DirEntry>>;
	/// Construction
	FileExplorerModel();
	///	\brief Initialization
	///	Loads the init file
	void Init() noexcept override;
	///	\brief Destruction
	///	Saves .ini file
	void Destroy() noexcept override;
	///	\brief get the current directory
	///	\return the current directory
	rxcpp::observable<std::string>		CurrentDirectoryFlowOut();
	///	\brief get the directory entries of the current directory
	///	\return a vector with the direcoru entries in the current directory
	rxcpp::observable<dir_entry_array>	CurrentDirEntriesFlowOut();
	///	\brief get the current directory's hierarchy
	///	\return the current directory;s hierarchy
	rxcpp::observable<dir_entry_array> CurrentDirHierarchyFlowOut();
	///	\brief get the number of hardware drives in the system
	///	\return the number of disk drives on the system
	rxcpp::observable<unsigned int>		DriveCountFlowOut();
	///	\brief Get the hardware drive letters
	///	\return a vector with the harware letters
	rxcpp::observable<drive_letter_array> DriveLettersFlowOut();
	///	\brief move to the parent directory
	void MoveUp() noexcept;
	///	\brief move into a directory
	void MoveInto(const std::string& path) noexcept;
	///	\brief
	void SetCurrentDrive(const std::string& letter);
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FileExplorerModel
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_MODELS_FILEEXPLORERMODEL_H__