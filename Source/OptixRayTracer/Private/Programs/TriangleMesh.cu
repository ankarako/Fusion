#include <optix_world.h>

rtBuffer<float3> 	vertex_buffer;
rtBuffer<float3> 	normal_buffer;
rtBuffer<float2>	texcoord_buffer;
rtBuffer<uint3>		tindex_buffer;
rtBuffer<int>		material_buffer;

rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );

///==========================================
///	Intersection Program for Triangle Meshes
///==========================================
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
			/// FIXME: make this right huh?
			rtReportIntersection(0);
		}
	}
}
///==========================================
///	Bounding Box program for Triangle Meshes
///==========================================
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