#ifndef	__IO_PUBLIC_ASSET_H__
#define __IO_PUBLIC_ASSET_H__

#include <MeshData.h>
#include <vector>
#include <memory>

namespace fu {
namespace io {

struct AssetObj
{
	bool HasBones{ false };
	bool HasAnimations{ false };
	std::vector<MeshData> Meshes;
};	///	!struct AsssetObj
///	\typedef Asset
///	\brief a reference counted AssetObj
using Asset = std::shared_ptr<AssetObj>;
///	\brief create an asset
static Asset CreateAsset()
{
	return std::make_shared<AssetObj>();
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_ASSET_H__