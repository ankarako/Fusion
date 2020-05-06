#ifndef	__OPTIXRAYTRACER_PUBLIC_COMPONENTS_POINTCLOUDCOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_POINTCLOUDCOMP_H__

#include <optix_world.h>
#include <memory>

namespace fu {
namespace rt {
///	\struct PointCloudCompObj
///	\brief a point cloud component
struct PointCloudCompObj
{
	optix::Geometry			Geometry;
	optix::Material 		Material;
	optix::GeometryInstance	GInstance;
	optix::GeometryGroup	GGroup;
	optix::Acceleration		Acceleration;
	optix::Transform		Transform;
	optix::Buffer			VertexBuffer;
	optix::Buffer			NormalBuffer;
	optix::Buffer			ColorBuffer;
	optix::Buffer			MaterialBuffer;
	optix::Program			IntersectionProg;
	optix::Program			BoundingBoxProgram;
	optix::Program			ClosestHitProgram;
	optix::Program			AnyHitProgram;
};	///	!struct PointCloudCompObj
///	\typedef PointCloudComp
///	\brief a reference counted point cloud component object
using PointCloudComp = std::shared_ptr<PointCloudCompObj>;
///	\brief create a point cloud component
static PointCloudComp CreatePointCloudComponent()
{
	return std::make_shared<PointCloudCompObj>();
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_POINTCLOUDCOMP_H__