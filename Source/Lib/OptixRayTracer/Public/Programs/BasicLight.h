#ifndef	__OPTIXRAYTRACER_PUBLIC_PROGRAMS_BASICLIGHT_H__
#define __OPTIXRAYTRACER_PUBLIC_PROGRAMS_BASICLIGHT_H__

#include <optix_world.h>

namespace fu {
namespace rt {

struct BasicLight
{
	optix::float3 Position;
	optix::float3 Color;
	int CastsShadow;
	int Padding;
};
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_PROGRAMS_BASICLIGHT_H__