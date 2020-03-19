#include <Models/RayTracingModel.h>
#include <Core/AssetTypeResolver.h>
#include <Components/ContextComp.h>
#include <Components/RaygenProgComp.h>
#include <Components/AccelerationComp.h>
#include <Components/TriangleMeshComp.h>
#include <Components/MeshMaterialComp.h> 
#include <Components/SolidColorMissComp.h>

#include <Systems/CreateContextSystem.h>
#include <Systems/RaygenSystem.h>
#include <Systems/MissSystem.h>
#include <Systems/MeshMappingSystem.h>
#include <Systems/LaunchSystem.h>

#include <LoadPly.h>
#include <LoadObj.h>

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
	std::vector<rt::TriangleMeshComp>	m_TriangleMeshComps;
	std::vector<rt::PointCloudComp>		m_PointCloudComps;
	/// our launch size
	unsigned int m_LaunchWidth;
	unsigned int m_LaunchHeight;

	rxcpp::subjects::subject<uint2>					m_LaunchSizeFlowInSubj;
	rxcpp::subjects::subject<BufferCPU<uchar4>>		m_FrameFlowOutSubj;
	rxcpp::subjects::subject<void*>					m_OnLaunchSubj;
	rxcpp::subjects::subject<mat_t>					m_RotationTransgformRaygenFlowInSubj;
	rxcpp::subjects::subject<vec_t>					m_TranslationRaygenFlowInSubj;
	rxcpp::subjects::subject<float>					m_CullingPlanePositionFlowInSubj;
	rxcpp::subjects::subject<io::MeshData>			m_MeshDataFlowInSubj;
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
	///================================
	/// Camera translation flow in task
	///================================
	m_Impl->m_TranslationRaygenFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](vec_t& transVec) 
	{
		optix::float3 trans;
		trans.x = transVec[0];
		trans.y = transVec[1];
		trans.z = transVec[2];
		rt::RaygenSystem::SetPinholeRaygenEyeTranslation(m_Impl->m_PinholeRaygenComp, trans);
		rt::LaunchSystem::Launch(m_Impl->m_ContextComp, m_Impl->m_LaunchWidth, m_Impl->m_LaunchHeight, 0);
		rt::LaunchSystem::CopyOutputBuffer(m_Impl->m_PinholeRaygenComp, m_Impl->m_FrameBuffer);
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameBuffer);
	});

	m_Impl->m_CullingPlanePositionFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](float cullPlanePos) 
	{
		for (int i = 0; i < m_Impl->m_PointCloudComps.size(); i++)
		{
			rt::MeshMappingSystem::SetCullingPlanePos(m_Impl->m_PointCloudComps[i], cullPlanePos);
		}
		rt::LaunchSystem::Launch(m_Impl->m_ContextComp, m_Impl->m_LaunchWidth, m_Impl->m_LaunchHeight, 0);
		rt::LaunchSystem::CopyOutputBuffer(m_Impl->m_PinholeRaygenComp, m_Impl->m_FrameBuffer);
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameBuffer);
	});
	///================================
	/// Mesh data flow in task
	///================================
	m_Impl->m_MeshDataFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](io::MeshData data) 
	{
		if (!data.HasFaces)
		{
			/// create a point cloud component
			rt::PointCloudComp pcComp = rt::CreatePointCloudComponent();
			rt::MeshMappingSystem::MapMeshDataToPointCloud(data, pcComp, m_Impl->m_ContextComp);
			/// attach component to top level acceleration
			rt::MeshMappingSystem::AttachPointCloudCompToTopLevelAcceleration(pcComp, m_Impl->m_AccelerationComp);
			m_Impl->m_PointCloudComps.emplace_back(pcComp);
			this->OnLaunch().on_next(nullptr);
		}
		else
		{
			/// triangle mesh
			rt::TriangleMeshComp trComp = rt::CreateTriangleMeshComponent();
			rt::MeshMappingSystem::MapMeshDataToTriangleMesh(data, trComp, m_Impl->m_ContextComp);
			if (m_Impl->m_TriangleMeshComps.size() == 1)
			{
				rt::MeshMappingSystem::DetachTriangleMeshToTopLevelAcceleration(m_Impl->m_TriangleMeshComps.back(), m_Impl->m_AccelerationComp);
				m_Impl->m_TriangleMeshComps.clear();
			}
			rt::MeshMappingSystem::AttachTriangleMeshToAcceleration(trComp, m_Impl->m_AccelerationComp);
			m_Impl->m_TriangleMeshComps.emplace_back(trComp);
			this->OnLaunch().on_next(nullptr);
		}
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
///
rxcpp::observable<BufferCPU<uchar4>> RayTracingModel::FrameFlowOut()
{
	return m_Impl->m_FrameFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observer<uint2> fu::fusion::RayTracingModel::LaunchSizeFlowIn()
{
	return m_Impl->m_LaunchSizeFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<void*> fu::fusion::RayTracingModel::OnLaunch()
{
	return m_Impl->m_OnLaunchSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<RayTracingModel::mat_t> fu::fusion::RayTracingModel::CameraRotationTransformFlowIn()
{
	return m_Impl->m_RotationTransgformRaygenFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<RayTracingModel::vec_t> fu::fusion::RayTracingModel::CameraTranslationFlowIn()
{
	return m_Impl->m_TranslationRaygenFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<float> fu::fusion::RayTracingModel::CullingPlanePositionFlowIn()
{
	return m_Impl->m_CullingPlanePositionFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<io::MeshData> fu::fusion::RayTracingModel::MeshDataFlowIn()
{
	return m_Impl->m_MeshDataFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

}	///	!namespace fusion
}	///	!namespace fu