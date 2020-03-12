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
}	///	!namespace rt
}	///	!namespace fu