#include <optix_world.h>
#include <Helpers.cuh>

rtBuffer<float3> 	vertex_buffer;
rtBuffer<float3> 	normal_buffer;
rtBuffer<uchar4>	color_buffer;
rtBuffer<float2>	texcoord_buffer;
rtBuffer<uint3>		tindex_buffer;

rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(optix::float2, cur_texcoord, 		attribute VertexTexcoord, );
rtDeclareVariable(optix::uchar4, cur_color,			attribute TriangleColor, );
rtDeclareVariable(optix::float3, cur_geom_normal,	attribute GeometricNormal, );
rtDeclareVariable(optix::float3, cur_shad_normal,	attribute ShadingNormal, );
rtDeclareVariable(optix::float3, back_hit_point,	attribute BackHitPoint, );
rtDeclareVariable(optix::float3, front_hit_point,	attribute FrontHitPoint, );

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
			///==========================
			/// Check for normal buffer
			///==========================
			if (normal_buffer.size() == 0)
			{
				cur_shad_normal = cur_geom_normal;
			}
			else
			{
				optix::float3 n0 = normal_buffer[vertexIdx.x];
				optix::float3 n1 = normal_buffer[vertexIdx.y];
				optix::float3 n2 = normal_buffer[vertexIdx.z];
				/// just for debuggin
				optix::float3 n = (n0 + n1 + n2) / 3;
				cur_geom_normal = optix::normalize(n);
				cur_shad_normal = optix::normalize(n1 * beta + n2 * gamma + n0 * (1.0f - beta - gamma));
				optix::float3 world_shading_normal = optix::normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, cur_geom_normal));
				//cur_color = fu::rt::make_color(world_shading_normal);
			}
			///===========================
			/// check for texcoord buffer
			///===========================
			if (texcoord_buffer.size() == 0)
			{
				cur_texcoord = make_float2(0.0f, 0.0f);
			}
			else
			{
				float2 t0 = texcoord_buffer[vertexIdx.x];
				float2 t1 = texcoord_buffer[vertexIdx.y];
				float2 t2 = texcoord_buffer[vertexIdx.z];
				cur_texcoord = t1 * beta + t2 * gamma + t0 * (1.0f - beta - gamma);
			}
			///=======================
			/// check for color buffer
			///=======================
			if (color_buffer.size() != 0)
			{
				optix::uchar4 c0 = color_buffer[vertexIdx.x];
				optix::uchar4 c1 = color_buffer[vertexIdx.y];
				optix::uchar4 c2 = color_buffer[vertexIdx.z];
				optix::float3 hitpoint = ray.origin + t * ray.direction;
				cur_color = fu::rt::triangle_color(v0, v1, v2, hitpoint, c0, c1, c2);
			}
			/// material is always zero
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
