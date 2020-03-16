#ifndef	__FUSION_PUBLIC_VIEWS_FILEEXPLORERVIEW_H__
#define __FUSION_PUBLIC_VIEWS_FILEEXPLORERVIEW_H__

#include <Core/DirEntry.h>
#include <Initializable.h>
#include <Widget.h>
#include <spimpl.h>
#include <vector>
#include <string>
#include <rxcpp/rx.hpp> 

namespace fu {
namespace app {
class FontManager;
}
namespace fusion {
enum class FileExplorerMode
{
	OpenProject,
	SaveProjectAs,
	SaveProject,
	OpenVideoFile,
	Open3DFile,
};	///	!enum FileExplorerCommand
///	\class FileExplorerView
///	\brief the file exploer widget
class FileExplorerView : public app::Widget, public app::Initializable
{
public:
	using drive_letter_array = std::shared_ptr<std::vector<std::string>>;
	using dir_entry_array	= std::shared_ptr<std::vector<DirEntry>>;
	using fman_ptr_t = std::shared_ptr<app::FontManager>;
	
	FileExplorerView(fman_ptr_t fman);

	void Init() override;
	void Render() override;
	
	void SetMode(FileExplorerMode mode);
	/// Inputs
	rxcpp::observer<std::string>		CurrentDirectoryFlowIn();
	rxcpp::observer<dir_entry_array>	CurrentDirEntriesFlowIn();
	rxcpp::observer<dir_entry_array>	CurrentDirHierarchyIn();
	rxcpp::observer<unsigned int>		DriveCountFlowIn();
	rxcpp::observer<drive_letter_array> DriveLettersFlowIn();
	rxcpp::observer<std::string>		CurrentSelectedDriveFlowIn();
	///	Outputs
	rxcpp::observable<std::string>		OpenProjectFileFlowOut();
	rxcpp::observable<DirEntry>			SaveProjectFileFlowOut();
	rxcpp::observable<std::string>		OpenVideoFileFlowOut();
	rxcpp::observable<std::string>		Open3DFileFlowOut();
	rxcpp::observable<void*>			OnMoveUpButtonClicked();
	rxcpp::observable<std::string>		OnMoveIntoDirectory();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FileExplorerVierw
}
}
#endif	///	!__FUSION_PUBLIC_VIEWS_FILEEXPLORERVIEW_H__