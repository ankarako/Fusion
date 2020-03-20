#ifndef __IO_PUBLIC_BONE_H__
#define __IO_PUBLIC_BONE_H__

#include <vector>
#include <string>

namespace fu {
namespace io {

struct BoneObj
{
	unsigned int 		NumWeights;
	std::vector<float> 	Weights;
};
}	///	!namespace io
}	///	!namespace fu
#endif	/// !__IO_PUBLIC_BONE_H__