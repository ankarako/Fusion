#ifndef	__FUSION_PUBLIC_CORE_DIRENTRY_H__
#define __FUSION_PUBLIC_CORE_DIRENTRY_H__

#include <string>

namespace fusion {
///	\struct DirEntry
///	\brief a directory entry
struct DirEntry
{
	///	\enum Type
	///	\brief the directory entry type
	enum class Type
	{
		Folder,
		File,
	};	///	!enum Type
	std::string Name;
	std::string AbsPath;
	Type EntryType;
};	///	!struct DirEntry
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_CORE_DIRENTRY_H__