#ifndef	__FUSION_PUBLIC_VIEWS_FILEEXPLORERVIEW_H__
#define __FUSION_PUBLIC_VIEWS_FILEEXPLORERVIEW_H__

#include <Core/DirEntry.h>
#include <Initializable.h>
#include <Widget.h>
#include <spimpl.h>
#include <vector>
#include <string>
#include <rxcpp/rx.hpp> 

namespace fusion {
///	\enum FileExplorerMode
///	\brief open or save
enum class FileExplorerMode
{
	Open,
	Save
};
///	\class FileExplorerView
///	\brief the file exploer widget
class FileExplorerView : public app::Widget, public app::Initializable
{
public:
	using drive_letter_array = std::shared_ptr<std::vector<std::string>>;
	using dir_entry_array	= std::shared_ptr<std::vector<DirEntry>>;

	FileExplorerView();

	void Init() override;
	void Render() override;
	
	void SetMode(FileExplorerMode mode);
	rxcpp::observer<std::string> CurrentDirectoryFlowIn();
	rxcpp::observer<dir_entry_array> CurrentDirEntriesFlowIn();
	rxcpp::observer<dir_entry_array> CurrentDirHierarchyIn();
	rxcpp::observer<unsigned int> DriveCountFlowIn();
	rxcpp::observer<drive_letter_array> DriveLettersFlowIn();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FileExplorerVierw
}
#endif	///	!__FUSION_PUBLIC_VIEWS_FILEEXPLORERVIEW_H__