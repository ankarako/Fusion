#ifndef __OPTIXRAYTRACER_PUBLIC_OPTIXRAYTRACER_H__
#define __OPTIXRAYTRACER_PUBLIC_OPTIXRAYTRACER_H__

#include <vector_types.h>
#include <vector_functions.h>

namespace ray {
///	\class RayTracer
///	\brief a ray tracing context
class RayTracer
{
public:
	void Initialize(uint2 size);
	void Launch();
	void SetViewportSize(uint2 size);
	void AddLight();
	void SetEnvironmentMap();
	void SetCamera();
private:
	
};	///	!class RayTracer
}	///	!namespace ray
#endif	///	!__OPTIXRAYTRACER_PUBLIC_OPTIXRAYTRACER_H__