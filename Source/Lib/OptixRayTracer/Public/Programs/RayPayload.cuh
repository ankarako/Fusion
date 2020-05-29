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
struct PerRayData_Texturing
{
	optix::uchar4	PixelValue;
	int				CameraId;
	float			CameraDist;
	optix::float2	TexCoord;
};	///	!struct PerRayData_Texturing
}	///	!namespace rt
}	///	!namespace fu