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
	/// \typedef asset_load_syst_ptr_t
	///	\brief a reference counted instance of the asset loading system
	using asset_load_syst_ptr_t = std::shared_ptr<rt::AssetLoadingSystem>;

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
	std::vector<rt::TriangleMeshComp>	m_TriangleMeshComps;
	std::vector<rt::PointCloudComp>		m_PointCloudComps;
	/// our launch size
	unsigned int m_LaunchWidth;
	unsigned int m_LaunchHeight;

	rxcpp::subjects::subject<uint2>					m_LaunchSizeFlowInSubj;
	rxcpp::subjects::subject<BufferCPU<uchar4>>		m_FrameFlowOutSubj;
	rxcpp::subjects::subject<rt::TriangleMeshComp>	m_TriangleMeshFlowInSubj;
	rxcpp::subjects::subject<rt::PointCloudComp>	m_PointCloudFlowIntSubj;
	rxcpp::subjects::subject<void*>					m_OnLaunchSubj;
	rxcpp::subjects::subject<mat_t>				m_RotationTransgformRaygenFlowInSubj;
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
	m_Impl->m_SolidColorMissComp = rt::CreateSolidColorMissComponent(optix::make_float3(0.1f, 0.1f, 0.1f));
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
		/// Launch on frame
		this->OnLaunch().on_next(nullptr);
	});
	///========================
	/// PointCloud flow in Task
	///========================
	m_Impl->m_PointCloudFlowIntSubj.get_observable().as_dynamic()
		.subscribe([this](rt::PointCloudComp comp) 
	{
		/// push to point cloud array
		m_Impl->m_PointCloudComps.emplace_back(comp);
		/// attach to our top object
		rt::MeshMappingSystem::AttachPointCloudToAcceleration(m_Impl->m_PointCloudComps.back(), m_Impl->m_AccelerationComp);
		///
		rt::MeshMappingSystem::AccelerationCompMapDirty(m_Impl->m_AccelerationComp);
		this->OnLaunch().on_next(nullptr);
	});
	///=============
	/// Launch task
	///=============
	m_Impl->m_OnLaunchSubj.get_observable().as_dynamic()
		.subscribe([this](auto _) 
	{
		rt::LaunchSystem::Launch(m_Impl->m_ContextComp, m_Impl->m_LaunchWidth, m_Impl->m_LaunchHeight, 0);
		rt::LaunchSystem::CopyOutputBuffer(m_Impl->m_PinholeRaygenComp, m_Impl->m_FrameBuffer);
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameBuffer);
	});
	///=============================
	///	Camera rotation flow in task
	///=============================
	m_Impl->m_RotationTransgformRaygenFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](mat_t mat) 
	{
		optix::Matrix4x4 trmat;
		std::memcpy(trmat.getData(), mat.data(), 16 * sizeof(float));
		rt::RaygenSystem::SetPinholeRaygenTransMat(m_Impl->m_PinholeRaygenComp, trmat);
		rt::LaunchSystem::Launch(m_Impl->m_ContextComp, m_Impl->m_LaunchWidth, m_Impl->m_LaunchHeight, 0);
		rt::LaunchSystem::CopyOutputBuffer(m_Impl->m_PinholeRaygenComp, m_Impl->m_FrameBuffer);
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameBuffer);
	});

}
///	\brief update the model
///	if the scene is valid it launches the context
void RayTracingModel::Update()
{
	if (m_Impl->m_IsValid)
	{
		
	}
}
///	\brief destroy the model
void RayTracingModel::Destroy()
{

}
void RayTracingModel::SetIsValid(bool val)
{
	m_Impl->m_IsValid = val;
}
bool RayTracingModel::GetIsValid() const
{
	return m_Impl->m_IsValid;
}
/// 
rt::ContextComp& RayTracingModel::GetCtxComp()
{
	return m_Impl->m_ContextComp;
}
///	\brief load a 3D asset
///	\param filepath the path to the file to load
void RayTracingModel::LoadAsset(const std::string& filepath)
{
	//m_Impl->m_AssetLoadSystem->LoadAsset(filepath, m_Impl->m_ContextComp);
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

rxcpp::observer<rt::TriangleMeshComp> fu::fusion::RayTracingModel::TriangleMeshCompFlowIn()
{
	return m_Impl->m_TriangleMeshFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<rt::PointCloudComp> fu::fusion::RayTracingModel::PointCloudFlowIn()
{
	return m_Impl->m_PointCloudFlowIntSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<void*> fu::fusion::RayTracingModel::OnLaunch()
{
	return m_Impl->m_OnLaunchSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<RayTracingModel::mat_t> fu::fusion::RayTracingModel::CameraRotationTransformFlowIn()
{
	return m_Impl->m_RotationTransgformRaygenFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu