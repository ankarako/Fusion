#ifndef __OPTIXRAYTRACER_PUBLIC_PROGRAMS_HELPERS_H__
#define __OPTIXRAYTRACER_PUBLIC_PROGRAMS_HELPERS_H__

#include <optix_world.h>

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
///	\brief get a triangle color given the colors of its vertices
///	\param	v0	the first vertex position of the triangle
///	\param	v1	the second vertex position of the triangle
///	\param	v2	the third verttex position of the triangle
///	\param	vc0	the color of the first vertex
///	\param	vc1	the color of the second vertex
///	\param	vc2	the color of the third vertex
///	\return the color of the triangle (uchar4)
static __device__ __inline__ const optix::uchar4 triangle_color(
	const optix::float3& v0,
	const optix::float3& v1,
	const optix::float3& v2,
	const optix::float3& p,
	const optix::uchar4& vc0,
	const optix::uchar4& vc1,
	const optix::uchar4& vc2)
{
	float v0v1v2 	= optix::length(optix::cross(v1 - v0, v2 - v1));
	float pv1v2 	= optix::length(optix::cross(v1 - p, v2 - v1));
	float pv2v0		= optix::length(optix::cross(v2 - p, v0 - v2));
	float pv0v1		= optix::length(optix::cross(v0 - p, v1 - v0));

	float c1 = pv1v2 / v0v1v2;
	float c2 = pv2v0 / v0v1v2;
	float c3 = 1.0f - c1 - c2;
	optix::float3 _vc0 = optix::make_float3(
		(float)vc0.x / 255.0f,
		(float)vc0.y / 255.0f,
		(float)vc0.z / 255.0f
	);
	optix::float3 _vc1 = optix::make_float3(
		(float)vc1.x / 255.0f,
		(float)vc1.y / 255.0f,
		(float)vc1.z / 255.0f
	);
	optix::float3 _vc2 = optix::make_float3(
		(float)vc2.x / 255.0f,
		(float)vc2.y / 255.0f,
		(float)vc2.z / 255.0f
	);
	optix::float3 color = c1 * _vc0 + c2 * _vc1 + c3 * _vc2;
	return make_color(color);
}

#endif	///	!__CUDACC__
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_PROGRAMS_HELPERS_H__