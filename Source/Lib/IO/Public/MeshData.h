#ifndef __IO_PUBLIC_MESHDATA_H__
#define __IO_PUBLIC_MESHDATA_H__

#include <Buffer.h>

namespace fu {
namespace io {

struct MeshDataObj
{
	BufferCPU<float3>	VertexBuffer;
	BufferCPU<float3> 	NormalBuffer;
	BufferCPU<uchar4> 	ColorBuffer;
	BufferCPU<uint3> 	TIndexBuffer;
	BufferCPU<float2>	TexcoordBuffer;
	bool HasNormals{ false };
	bool HasColors{ false };
	bool HasTexcoords{ false }; 
	bool HasFaces{ false };
};	///	!struct MeshData
///	\typedef MeshData
///	\brief a reference counted MeshData object
using MeshData = std::shared_ptr<MeshDataObj>;
///	\brief create a mesh data 
static MeshData CreateMeshData()
{
	return std::make_shared<MeshDataObj>();
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_MESHDATA_H__