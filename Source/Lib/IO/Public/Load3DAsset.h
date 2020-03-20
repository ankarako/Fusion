#ifndef __IO_PUBLIC_LOAD3DASSET_H__
#define __IO_PUBLIC_LOAD3DASSET_H__

#include <MeshData.h>
#include <string>



namespace fu {
namespace io {
/// Load a 3D asset and get its mesh data
MeshData Load3DAsset(const std::string& filepath);
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_LOAD3DASSET_H__