#include <Models/RayTracingModel.h>
#include <Components/ContextComp.h>
#include <Components/RaygenProgComp.h>
#include <Components/AccelerationComp.h>
#include <Components/TriangleMeshComp.h>
#include <Components/MeshMaterialComp.h>

#include <vector>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief RayTracer Implementation
struct RayTracingModel::Impl
{
	/// ray tracing context component
	///	video tracer and raytracer use different contexts
	rt::ContextComp			m_ContextComp;
	///	ray generation program component
	rt::RaygenProgComp 		m_PinholeRaygenComp;
	/// top acceleration component
	rt::AccelerationComp 	m_AccelerationComp;
	/// our 3D meshes
	std::vector<rt::TriangleMeshComp> m_TriangleMeshComp;
	/// our launch size
	unsigned int m_LauchWidth;
	unsigned int m_LaunchHeight;
	/// Construction
	Impl() { }
};	///	!struct Impl
/// Construction

}	///	!namespace fusion
}	///	!namespace fu