#ifndef __OPTIXRAYTRACER_PUBLIC_PROGRAMS_HELPERS_H__
#define __OPTIXRAYTRACER_PUBLIC_PROGRAMS_HELPERS_H__

#include <optixu/optixu_math_namespace.h>

namespace fu {
namespace rt {
#ifdef __CUDACC__
///	\brief convert a float3 to uchar3
///	\param	c	the float3 color
///	\note: only for cuda
static __device__ __inline__ optix::uchar4 make_color(const optix::float3& c)
{
	return optix::make_uchar4(
		static_cast<unsigned char>(__saturatef(c.x)*255.99f),  /* R */
		static_cast<unsigned char>(__saturatef(c.y)*255.99f),  /* G */
		static_cast<unsigned char>(__saturatef(c.z)*255.99f),  /* B */
        255u);                                                 /* A */
}
#endif	///	!__CUDACC__

}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_PROGRAMS_HELPERS_H__