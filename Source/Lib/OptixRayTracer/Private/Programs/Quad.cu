#include <optix_world.h>
#include <Helpers.cuh>

rtBuffer<optix::float3> vertex_buffer;
rtDeclareVariable(optix::float3, color, attribute quad_color, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );

RT_PROGRAM void quad_intersect(int primIdx)
{
	
	optix::float3 v0 = vertex_buffer[0];
	optix::float3 v1 = vertex_buffer[1];
	optix::float3 v2 = vertex_buffer[2];
	optix::float3 v3 = vertex_buffer[3];

	optix::float3 normal;
	float t;
	float beta;
	float gamma;
	if (optix::intersect_triangle(ray, v0, v1, v3, normal, t, beta, gamma) || optix::intersect_triangle(ray, v0, v2, v3, normal, t, beta, gamma))
	{
		if (rtPotentialIntersection(t))
		{
			rtReportIntersection(0);
		}
		
	}
}

RT_PROGRAM void quad_bounds(int primIdx, float result[6])
{
	const optix::float3 v0 = vertex_buffer[0];
	const optix::float3 v1 = vertex_buffer[1];
	const optix::float3 v2 = vertex_buffer[2];
	const optix::float3 v3 = vertex_buffer[3];

	optix::Aabb* aabb = (optix::Aabb*)result;

	aabb->m_min = fminf(fminf(v0, v1), fminf(v2, v3));
	aabb->m_max = fmaxf(fmaxf(v0, v1), fmaxf(v2, v3));
}