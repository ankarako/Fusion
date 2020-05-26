#include <optix_world.h>
/// inputs
rtDeclareVariable(optix::float4, plane, , );
rtDeclareVariable(optix::float3, v1, , );
rtDeclareVariable(optix::float3, v2, , );
rtDeclareVariable(optix::float3, anchor, , );
rtDeclareVariable(int, lgt_instance, , ) = { 0 };
/// attributes
rtDeclareVariable(optix::float3, texcoord, attribute texcoord, );
rtDeclareVariable(optix::float3, geometric_normal, attribute geometric_normal, );
rtDeclareVariable(optix::float3, shading_normal, attribute shading_normal, );
rtDeclareVariable(int, lgt_idx, attribute lgt_idx, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );

RT_PROGRAM void parallelogram_intersect(int primIdx)
{
	optix::float3 n = optix::make_float3(plane);
	float dt = optix::dot(ray.direction, n);
	float t = (plane.w - optix::dot(n, ray.origin)) / dt;
	
	if (t > ray.tmin && t < ray.tmax)
	{
		optix::float3 p = ray.origin + t * ray.direction;
		optix::float3 vi = p - anchor;
		float a1 = optix::dot(v1, vi);
		
		if (a1 >= 0 && a1 <= 1)
		{
			float a2 = optix::dot(v1, vi);

			if (a2 >= 0 && a2 <= 1)
			{
				if (rtPotentialIntersection(t))
				{
					shading_normal = geometric_normal = n;
					texcoord = optix::make_float3(a1, a2, 0);
					lgt_idx = lgt_instance;
					rtReportIntersection(0);
				}
			}
		}
	}
}

RT_PROGRAM void parallelogram_bounds(int, float result[6])
{
	const optix::float3 tv1 = v1 / optix::dot(v1, v1);
	const optix::float3 tv2 = v2 / optix::dot(v2, v2);
	const optix::float3 p00 = anchor;
	const optix::float3 p01 = anchor + tv1;
	const optix::float3 p10 = anchor + tv2;
	const optix::float3 p11 = anchor + tv1 + tv2;
	const float area = optix::length(optix::cross(tv1, tv2));

	optix::Aabb* aabb = (optix::Aabb*)result;

	if (area > 0.0f && !isinf(area))
	{
		aabb->m_min = fminf(fminf(p00, p01), fminf(p10, p11));
		aabb->m_max = fmaxf(fmaxf(p00, p01), fmaxf(p10, p11));
	}
	else
	{
		aabb->invalidate();
	}
}