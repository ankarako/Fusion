#ifndef __OPTIXRAYTRACER_PUBLIC_OPTIXRAYTRACER_H__
#define __OPTIXRAYTRACER_PUBLIC_OPTIXRAYTRACER_H__

#include <vector_types.h>
#include <vector_functions.h>
#include <optix.h>
#include <optixu/optixpp_namespace.h>

namespace ray {
///	\struct RayTracerState
///	\brief the ray tracer's state
struct RayTracerState
{
	optix::Context Context;
	
};	///	!struct RayTracerState
}	///	!namespace ray
#endif	///	!__OPTIXRAYTRACER_PUBLIC_OPTIXRAYTRACER_H__