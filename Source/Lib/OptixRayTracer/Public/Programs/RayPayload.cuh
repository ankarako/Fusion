#ifndef __OPTIXRAYTRACER_PUBLIC_PROGRAMS_RAYPAYLOAD_CUH__
#define __OPTIXRAYTRACER_PUBLIC_PROGRAMS_RAYPAYLOAD_CUH__

#include <optix_world.h>

namespace fu {
namespace rt {
///	\struct PerRayData_Radiance
///	\brief ray's payload
struct PerRayData_Radiance
{
	optix::uchar4 	Result;
	float			Importance;
	int				Depth;
};	///	!struct PerRayData_Radiance
///	\struct PerRayData_Texturing
///	\brief ray payload for texturing
struct TexturingOutput
{
	optix::uchar4	Color;
	optix::float2	Texcoord;
	float			Weight;
	int				CamId;
};

struct PerRayData_Texturing
{
	optix::uchar4	PixelValue;
	int				CameraId;
	float			CameraDist;
	TexturingOutput Output;
};	///	!struct PerRayData_Texturing
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_PROGRAMS_RAYPAYLOAD_CUH__