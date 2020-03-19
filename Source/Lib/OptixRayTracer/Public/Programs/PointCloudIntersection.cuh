#ifndef	__OPTIXRAYTRACER_PUBLIC_PROGRAMS_POINTCLOUDINTERSECTION_CUH__
#define __OPTIXRAYTRACER_PUBLIC_PROGRAMS_POINTCLOUDINTERSECTION_CUH__

#include <optix_world.h>
#include <Helpers.cuh>

#ifdef __CUDACC__

namespace fu {
namespace rt {
/// point cloud intersection
__device__ __inline__ bool pointcloud_intersection(optix::Ray ray, optix::float3 pos, float radius)
{
	const float rad = radius;
	optix::float3 pos_orig_vec = ray.origin - pos;
	float b = optix::dot(pos_orig_vec, ray.direction);
	float c = optix::dot(pos_orig_vec, pos_orig_vec) - rad * rad;
	float disc = b * b - c;

	if (disc > 0.0f)
	{
		float sdisc = sqrtf(disc);
		float root1 = (-b - sdisc);
		bool check_second = true;
		if (rtPotentialIntersection(root1))
		{
			if (rtReportIntersection(0))
			{
				check_second = false;
			}
			if (check_second)
			{
				float root2 = -b + sdisc;
				if (rtPotentialIntersection(root2))
				{
					return true;
				}
			}
		}
	}
	return false;
}

/// point cloud intersection
__device__ __inline__ bool pointcloud_intersection_color(optix::Ray ray, optix::float3 pos, float radius)
{
	const float rad = radius;
	optix::float3 pos_orig_vec = ray.origin - pos;
	float b = optix::dot(pos_orig_vec, ray.direction);
	float c = optix::dot(pos_orig_vec, pos_orig_vec) - rad * rad;
	float disc = b * b - c;

	if (disc > 0.0f)
	{
		float sdisc = sqrtf(disc);
		float root1 = (-b - sdisc);
		bool check_second = true;
		if (rtPotentialIntersection(root1))
		{
			if (rtReportIntersection(0))
			{
				check_second = false;
			}
			if (check_second)
			{
				float root2 = -b + sdisc;
				if (rtPotentialIntersection(root2))
				{
					return true;
				}
			}
		}
	}
	return false;
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__CUDACC__
#endif	///	__OPTIXRAYTRACER_PUBLIC_PROGRAMS_POINTCLOUDINTERSECTION_CUH__