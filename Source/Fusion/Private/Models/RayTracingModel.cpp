#include <Models/RayTracingModel.h>
#include <Components/ContextComp.h>
#include <Components/RaygenProgComp.h>
#include <Components/AccelerationComp.h>
#include <Components/TriangleMeshComp.h>
#include <Components/MeshMaterialComp.h> 
#include <Components/SolidColorMissComp.h>

#include <Systems/AssetLoadingSystem.h>
#include <Systems/CreateContextSystem.h>
#include <Systems/RaygenSystem.h>
#include <Systems/MissSystem.h>
#include <Systems/MeshMappingSystem.h>
#include <Systems/LaunchSystem.h>

#include <vector>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief RayTracer Implementation
struct RayTracingModel::Impl
{
	bool					m_IsValid{ false };
	BufferCPU<uchar4>		m_FrameBuffer;
	/// ray tracing context component
	///	video tracer and raytracer use different contexts
	rt::ContextComp			m_ContextComp;
	///	ray generation program component
	rt::RaygenProgComp 		m_PinholeRaygenComp;
	/// top acceleration component
	rt::AccelerationComp 	m_AccelerationComp;
	/// miss program - solid color background
	rt::SolidColorMissComp	m_SolidColorMissComp;
	/// our 3D meshes
	std::vector<rt::TriangleMeshComp> m_TriangleMeshComps;
	/// our launch size
	unsigned int m_LaunchWidth;
	unsigned int m_LaunchHeight;

	rxcpp::subjects::subject<uint2>				m_LaunchSizeFlowInSubj;
	rxcpp::subjects::subject<BufferCPU<uchar4>> m_FrameFlowOutSubj;
	/// Construction
	Impl() { }
};	///	!struct Impl
/// Construction
RayTracingModel::RayTracingModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
///	\brief initialize the model
void RayTracingModel::Init()
{
	/// create contenxt component
	m_Impl->m_ContextComp = rt::CreateContextComponent();
	rt::CreateContextSystem::CreateContext(m_Impl->m_ContextComp, 1, 1);
	/// create a miss program
	m_Impl->m_SolidColorMissComp = rt::CreateSolidColorMissComponent(optix::make_float3(0.7f, 0.1f, 0.1f));
	rt::MissSystem::InitializeSolidColorComp(m_Impl->m_SolidColorMissComp, m_Impl->m_ContextComp);
	/// create pinhole raygen program
	m_Impl->m_PinholeRaygenComp = rt::CreateRaygenProgComponent();
	/// TODO: dimensions
	rt::RaygenSystem::CreatePinholeRaygenProg(m_Impl->m_PinholeRaygenComp, m_Impl->m_ContextComp, 1920, 1080);
	/// place a dummy triangle mesh component
	m_Impl->m_TriangleMeshComps.emplace_back(rt::CreateTriangleMeshComponent());
	/// initialize the triangle mesh comp with no data
	rt::MeshMappingSystem::NullInitializeTriangleMesh(m_Impl->m_TriangleMeshComps.back(), m_Impl->m_ContextComp);
	/// create an acceleration component
	m_Impl->m_AccelerationComp = rt::CreateAccelerationComponent();
	rt::MeshMappingSystem::NullInitializeAcceleration(m_Impl->m_AccelerationComp, m_Impl->m_ContextComp);
	/// attach mesh to acceleration
	rt::MeshMappingSystem::AttachTriangleMeshToAcceleration(m_Impl->m_TriangleMeshComps.back(), m_Impl->m_AccelerationComp);
	rt::MissSystem::AttachSolidColorMissToContext(m_Impl->m_SolidColorMissComp, m_Impl->m_ContextComp);
	///==========================
	/// launch size flow in task
	///==========================
	m_Impl->m_LaunchSizeFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](uint2 size) 
	{
		m_Impl->m_LaunchWidth = size.x;
		m_Impl->m_LaunchHeight = size.y;
		rt::RaygenSystem::CreatePinholeRaygenProg(m_Impl->m_PinholeRaygenComp, m_Impl->m_ContextComp, size.x, size.y);
		rt::RaygenSystem::SetRaygenAttributes(m_Impl->m_PinholeRaygenComp);
		rt::MeshMappingSystem::MapAccelerationToRaygen(m_Impl->m_AccelerationComp, m_Impl->m_PinholeRaygenComp);

		m_Impl->m_FrameBuffer = CreateBufferCPU<uchar4>(m_Impl->m_LaunchWidth * m_Impl->m_LaunchHeight);
		m_Impl->m_IsValid = true;
	});
	
}
///	\brief update the model
///	if the scene is valid it launches the context
void RayTracingModel::Update()
{
	if (m_Impl->m_IsValid)
	{
		rt::LaunchSystem::Launch(m_Impl->m_ContextComp, m_Impl->m_LaunchWidth, m_Impl->m_LaunchHeight, 0);
		rt::LaunchSystem::CopyOutputBuffer(m_Impl->m_PinholeRaygenComp, m_Impl->m_FrameBuffer);
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameBuffer);
	}
}
///	\brief destroy the model
void RayTracingModel::Destroy()
{

}
///	\brief load a 3D asset
///	\param filepath the path to the file to load
void RayTracingModel::LoadAsset(const std::string& filepath)
{

}
///
rxcpp::observable<BufferCPU<uchar4>> RayTracingModel::FrameFlowOut()
{
	return m_Impl->m_FrameFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observer<uint2> fu::fusion::RayTracingModel::LaunchSizeFlowIn()
{
	return m_Impl->m_LaunchSizeFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu