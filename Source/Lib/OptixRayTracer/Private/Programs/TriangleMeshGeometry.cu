#include <optix_world.h>
#include <Helpers.cuh>

rtBuffer<float3> vertex_buffer;
rtBuffer<uint3> tindex_buffer;

rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(optix::float3, geometric_normal, attribute GeometricNormal, );
rtDeclareVariable(optix::float3, shading_normal, attribute ShadingNormal, );
rtDeclareVariable(optix::float3, back_hit_point, attribute BackHitPoint, );
rtDeclareVariable(optix::float3, front_hit_point, attribute FrontHitPoint, );
///=============
/// Intersection
///=============
RT_PROGRAM void triangle_mesh_intersect(int primIdx)
{
	const uint3 vertexIdx = tindex_buffer[primIdx];
	const float3 v0 = vertex_buffer[vertexIdx.x];
	const float3 v1 = vertex_buffer[vertexIdx.y];
	const float3 v2 = vertex_buffer[vertexIdx.z];

	float3 normal;
	float t;
	float beta;
	float gamma;
	if (optix::intersect_triangle(ray, v0, v1, v2, normal, t, beta, gamma))
	{
		if (rtPotentialIntersection(t))
		{
			geometric_normal = optix::normalize(normal);
			shading_normal = geometric_normal;
			rtPotentialIntersection(0);
		}
	}
}
///==============
/// BBox Program
///==============
RT_PROGRAM void triangle_mesh_bounds(int primIdx, float result[6])
{
	const uint3 vertexIdx = tindex_buffer[primIdx];

	const float3 v0 = vertex_buffer[vertexIdx.x];
	const float3 v1 = vertex_buffer[vertexIdx.y];
	const float3 v2 = vertex_buffer[vertexIdx.z];
	const float area = optix::length(optix::cross(v1 - v0, v2 - v0));
	
	optix::Aabb* aabb = (optix::Aabb*)result;
	if (area > 0.0f && !isinf(area))
	{
		aabb->m_min = fminf(fminf(v0, v1), v2);
		aabb->m_max = fmaxf(fmaxf(v0, v1), v2);
	}
	else
	{
		aabb->invalidate();
	}
}