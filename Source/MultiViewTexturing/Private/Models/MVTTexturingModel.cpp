#include <Models/MVTTexturingModel.h>
#include <Components/AccelerationComp.h>
#include <Components/ContextComp.h>
#include <Components/TriangleMeshComp.h>
#include <Components/TexturingCameraComp.h>
#include <Components/SolidColorMissComp.h>
#include <Systems/CreateContextSystem.h>
#include <Systems/MeshMappingSystem.h>
#include <Systems/LaunchSystem.h>
#include <Systems/RaygenSystem.h>
#include <Systems/MissSystem.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief MVTModel implementation
struct MVTModel::Impl
{
	rt::ContextComp							m_ContextComp;
	rt::AccelerationComp					m_TopLevelAcceleration;
	rt::TriangleMeshComp					m_TriangleMeshComp;
	rt::SolidColorMissComp					m_MissComp;
	std::vector<rt::TexturingCameraComp>	m_TexturingCameraComps;
	rxcpp::subjects::subject<io::MeshData>								m_MeshDataFlowInSubj;
	rxcpp::subjects::subject<std::vector<io::volcap_cam_data_ptr_t>>	m_CameraDataFlowInSubj;
	rxcpp::subjects::subject<std::vector<BufferCPU<uchar4>>>			m_CameraFramesFlowInSubj;
	rxcpp::subjects::subject<io::TrackedSequence>						m_TrackedFramesFlowInSubj;
	rxcpp::subjects::subject<io::perfcap_skeleton_ptr_t>				m_SkeletonFlowInSubj;
	rxcpp::subjects::subject<io::perfcap_skin_data_ptr_t>				m_SkinningDataFlowInSubj;
	rxcpp::subjects::subject<void*>										m_RunTexturingLoopSubj;
	rxcpp::subjects::subject<int>										m_SeekFrameFlowOutSubj;
	/// Construction
	Impl() 
		: m_ContextComp(rt::CreateContextComponent())
		, m_TopLevelAcceleration(rt::CreateAccelerationComponent())
		, m_TriangleMeshComp(rt::CreateTriangleMeshComponent())
		, m_MissComp(rt::CreateSolidColorMissComponent(optix::make_float3(0.0f, 1.0f, 0.0f)))
	{ }
};	///	!struct Impl
///	Construction
MVTModel::MVTModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
/// \brief model initialization
void MVTModel::Init()
{
	///==================
	/// CameraData FlowIn
	///==================
	m_Impl->m_CameraDataFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::vector<io::volcap_cam_data_ptr_t>& cam_data) 
	{
		int camCount = cam_data.size();
		rt::CreateContextSystem::CreateContext(m_Impl->m_ContextComp, camCount, 1);
		rt::MissSystem::InitializeSolidColorComp(m_Impl->m_MissComp, m_Impl->m_ContextComp);
		rt::MissSystem::AttachSolidColorMissToContext(m_Impl->m_MissComp, m_Impl->m_ContextComp);
		rt::MeshMappingSystem::NullInitializeAcceleration(m_Impl->m_TopLevelAcceleration, m_Impl->m_ContextComp);
		for (int c = 0; c < cam_data.size(); ++c)
		{
			io::volcap_cam_data_ptr_t cam = cam_data[c];
			rt::TexturingCameraComp comp = rt::CreateTexturingCameraComponent();
			comp->Extrinsics = optix::Matrix4x4(cam->ColorExtrinsics->Data());
			/// was column major
			comp->Extrinsics.transpose();
			comp->Intrinsics = optix::Matrix3x3(cam->ColorIntrinsics->Data());
			/// was column major
			comp->Intrinsics.transpose();
			optix::uint2 size = optix::make_uint2(1280, 720);
			rt::RaygenSystem::MapTexturingCamera(comp, m_Impl->m_ContextComp, size, c);
			rt::RaygenSystem::AttachTopLevelAccelerationToTexturingRaygen(comp, m_Impl->m_TopLevelAcceleration);
			m_Impl->m_TexturingCameraComps.emplace_back(comp);
		}
	});
	///================
	/// MeshData FlowIn
	///================
	m_Impl->m_MeshDataFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const io::MeshData& data) 
	{
		m_Impl->m_TriangleMeshComp = rt::CreateTriangleMeshComponent();
		rt::MeshMappingSystem::MapMeshDataToTriangleMesh(data, m_Impl->m_TriangleMeshComp, m_Impl->m_ContextComp);
		rt::MeshMappingSystem::AttachTriangleMeshToAcceleration(m_Impl->m_TriangleMeshComp, m_Impl->m_TopLevelAcceleration);
	});
	///==============
	/// Launch Task
	///==============
	m_Impl->m_RunTexturingLoopSubj.get_observable().as_dynamic()
		.subscribe([this](auto _) 
	{
	
	});

}
rxcpp::observer<io::MeshData> MVTModel::MeshDataFlowIn()
{
	return m_Impl->m_MeshDataFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<std::vector<io::volcap_cam_data_ptr_t>> MVTModel::CameraDataFlowIn()
{
	return m_Impl->m_CameraDataFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<std::vector<BufferCPU<uchar4>>> MVTModel::CameraFramesFlowIn()
{
	return m_Impl->m_CameraFramesFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<io::TrackedSequence> MVTModel::TrackedFramesFlowIn()
{
	return m_Impl->m_TrackedFramesFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<io::perfcap_skeleton_ptr_t> MVTModel::SkeletonFlowIn()
{
	return m_Impl->m_SkeletonFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<io::perfcap_skin_data_ptr_t> MVTModel::SkinningDataFlowIn()
{
	return m_Impl->m_SkinningDataFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<void*> MVTModel::RunTexturingLoop()
{
	return m_Impl->m_RunTexturingLoopSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observable<int> MVTModel::SeekFrameFlowOut()
{
	return m_Impl->m_SeekFrameFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace mvt
}	///	!namespace fu