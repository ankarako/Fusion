#include <optix_world.h>
#include <RayPayload.cuh>
// #include <PointCloudIntersection.cuh>

rtDeclareVariable(float, radius, , );
rtDeclareVariable(optix::float3, position, attribute point_position, );
rtDeclareVariable(optix::uchar4, color, attribute point_color, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(fu::rt::PerRayData_Radiance, prd_radiance, rtPayload, );
rtDeclareVariable(int, flat_shaded, , );
rtDeclareVariable(float, culling_plane_pos, , );

rtBuffer<optix::float3> vertex_positions;
rtBuffer<optix::uchar4> vertex_colors;

///=================================
///	Point cloud intersection program
///==================================
RT_PROGRAM void pointcloud_intersect(int primIdx)
{
	// const optix::float3 pos = vertex_positions[primIdx];
	// if (fu::rt::pointcloud_intersection(ray, pos, radius))
	// {
	// 	position = pos;
	// 	color = vertex_colors[primIdx];
	// 	rtReportIntersection(0);
	// }
	const optix::float3 pos = vertex_positions[primIdx];
	if (-pos.y < culling_plane_pos)
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
				position = pos;
				color = vertex_colors[primIdx];
				if (rtReportIntersection(0))
				{
					check_second = false;
				}
				if (check_second)
				{
					float root2 = -b + sdisc;
					if (rtPotentialIntersection(root2))
					{
						position = pos;
						color = vertex_colors[primIdx];
						rtReportIntersection(0);
					}
				}
			}
		}
	}
}
///===================================
/// Point cloud bounding box program
///===================================
RT_PROGRAM void pointcloud_bounds(int primIdx, float result[6])
{
	const optix::float3 pos = vertex_positions[primIdx];
	const float rad = radius;
	optix::Aabb* aabb = (optix::Aabb*)result;
	aabb->m_min.x = pos.x - rad;
	aabb->m_min.y = pos.y - rad;
	aabb->m_min.z = pos.z - rad;
	aabb->m_max.x = pos.x + rad;
	aabb->m_max.y = pos.y + rad;
	aabb->m_max.z = pos.z + rad;
}
///==============================
///	point cloud closest hit
///==============================
RT_PROGRAM void pointcloud_closest_hit()
{
	if (flat_shaded)
	{
		prd_radiance.Result = color;
	}
}
///====================
/// point cloud any hit
///====================
RT_PROGRAM void pointcloud_any_hit()
{
	// rtTerminateRay();
}