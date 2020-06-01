#ifndef __IO_PUBLIC_VOLCAPCAMERADATA_H__
#define __IO_PUBLIC_VOLCAPCAMERADATA_H__

#include <Buffer.h>
#include <string>
#include <memory>

namespace fu {
namespace io {
///	\struct VolcapCameraData
///	\brief	Volumetric Capture's (& performance capture's) camera related data
struct VolcapCameraData
{
	std::string DeviceName;
	uint2		DepthResolution;
	uint2		ColorResolution;
	BufferCPU<float> DepthIntrinsics;
	BufferCPU<float> DepthExtrinsics;
	BufferCPU<float> ColorIntrinsics;
	BufferCPU<float> ColorExtrinsics;
	/// TODO: Should I add Distortions coeffs?
};	///	!struct VolcapCameraData
///	\typedef volcap_cam_data_ptr_t
///	\brief a shared pointer to a VolcapCameraData object
using volcap_cam_data_ptr_t = std::shared_ptr<VolcapCameraData>;
///	\brief create 
static volcap_cam_data_ptr_t CreateVolcapCameraData()
{
	return std::make_shared<VolcapCameraData>();
}
}	///	!namespace io
}	///	!namesapce fu
#endif	///	!__IO_PUBLIC_VOLCAPCAMERADATA_H__