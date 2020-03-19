#ifndef	__FUSION_PUBLIC_CORE_ASSETTYPERESOLVER_H__
#define __FUSION_PUBLIC_CORE_ASSETTYPERESOLVER_H__

#include <map>
#include <string>
#include <filesystem>

namespace fu {
namespace fusion {
enum class AssetType
{
	Unknown,
	Ply,
	Obj,
};	///	!AssetType
namespace detail {
const std::map<std::string, AssetType> k_Extension2Type =
{
	{".obj", AssetType::Obj},
	{".OBJ", AssetType::Obj},
	{".ply", AssetType::Ply}
};
}


class AssetTypeResolver
{	
public:
	static AssetType GetAssetType(const std::string& filepath)
	{
		std::string ext = std::filesystem::path(filepath).extension().generic_string();
		auto& it = detail::k_Extension2Type.find(ext);
		return (it == detail::k_Extension2Type.end()) ? AssetType::Unknown : detail::k_Extension2Type.at(ext);
	}
};	///	!AssetTypeResolver
}	///	!namaspace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_CORE_ASSETTYPERESOLVER_H__