#include <Models/ViewportTracingModel.h>
#include <Components/AccelerationComp.h>
#include <Components/RaygenProgComp.h>
#include <Components/SolidColorMissComp.h>
#include <Components/TriangleMeshComp.h>
#include <Components/ContextComp.h>
#include <Components/ViewportFrustrumComp.h>
#include <Systems/RaygenSystem.h>
#include <Systems/MissSystem.h>
#include <Systems/CreateContextSystem.h>
#include <Systems/MeshMappingSystem.h>
#include <Systems/LaunchSystem.h>

#include <vector>
#include <array>
#include <DebugMsg.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief Viewport tracing model implmentation
struct ViewportTracingModel::Impl
{
	viewport_size_t							m_ViewportSize;
	BufferCPU<uchar4>						m_FrameBuffer;
	rt::ContextComp							m_ContextComp;
	rt::RaygenProgComp						m_PinholeRaygenComp;
	rt::SolidColorMissComp					m_SolidColorMissComp;
	rt::AccelerationComp					m_TopLevelAccelerationComp;
	std::vector<rt::TriangleMeshComp>		m_TriangleMeshComps;
	std::vector<rt::ViewportFrustrumComp>	m_ViewportFrustrumComps;
	/// IO
	rxcpp::subjects::subject<viewport_size_t>							m_ViewportSizeFlowInSubj;
	rxcpp::subjects::subject<std::vector<io::volcap_cam_data_ptr_t>>	m_CameraDataFlowInSubj;
	rxcpp::subjects::subject<std::vector<BufferCPU<uchar4>>>			m_VideoTexturesFlowInSubj;
	rxcpp::subjects::subject<io::MeshData>								m_MeshDataFlowInSubj;
	rxcpp::subjects::subject<mat_t>										m_CameraRotationTransformFlowInSubj;
	rxcpp::subjects::subject<trans_vec_t>								m_CameraTranslationVectorFlowInSubj;
	/// outputs
	rxcpp::subjects::subject<BufferCPU<uchar4>>							m_FrameBufferFlowOutSubj;
	/// events
	rxcpp::subjects::subject<void*>										m_LaunchContextTaskSubj;
	Impl()
		: m_ContextComp(rt::CreateContextComponent())
		, m_PinholeRaygenComp(rt::CreateRaygenProgComponent())
		, m_SolidColorMissComp(rt::CreateSolidColorMissComponent(optix::make_float3(0.09f, 0.09f, 0.09f)))
		, m_TopLevelAccelerationComp(rt::CreateAccelerationComponent())
	{ }
};
/// Construction
ViewportTracingModel::ViewportTracingModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
/// \brief model initialization
void ViewportTracingModel::Init()
{
	/// Create the ray tracing context
	rt::CreateContextSystem::CreateContext(m_Impl->m_ContextComp, 1, 1);
	/// Create the raygen component
	/// dummy raygen dimensions
	rt::RaygenSystem::CreatePinholeRaygenProg(m_Impl->m_PinholeRaygenComp, m_Impl->m_ContextComp, 10, 10);
	rt::RaygenSystem::SetRaygenAttributes(m_Impl->m_PinholeRaygenComp);
	/// miss program
	rt::MissSystem::InitializeSolidColorComp(m_Impl->m_SolidColorMissComp, m_Impl->m_ContextComp);
	rt::TriangleMeshComp comp = rt::CreateTriangleMeshComponent();
	rt::MeshMappingSystem::NullInitializeTriangleMesh(comp, m_Impl->m_ContextComp);
	/// Top level acceleration
	rt::MeshMappingSystem::NullInitializeAcceleration(m_Impl->m_TopLevelAccelerationComp, m_Impl->m_ContextComp);
	rt::MeshMappingSystem::AttachTriangleMeshToAcceleration(comp, m_Impl->m_TopLevelAccelerationComp);
	m_Impl->m_TriangleMeshComps.emplace_back(comp);
	/// attach top level acceleration
	rt::MissSystem::AttachSolidColorMissToContext(m_Impl->m_SolidColorMissComp, m_Impl->m_ContextComp);
	rt::MeshMappingSystem::MapAccelerationToRaygen(m_Impl->m_TopLevelAccelerationComp, m_Impl->m_PinholeRaygenComp);
	/// Initialize PinholeCamera and miss programs
	///=======================
	/// Viewport size flow in
	///=======================
	m_Impl->m_ViewportSizeFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const viewport_size_t& size) 
	{
		DebugMsg("Viewport Size updated");
		m_Impl->m_ViewportSize = size;
		rt::RaygenSystem::ChangePinholeRaygenDimensions(m_Impl->m_PinholeRaygenComp, size[0], size[1]);
		m_Impl->m_FrameBuffer = CreateBufferCPU<uchar4>(size[0] * size[1]);
		m_Impl->m_LaunchContextTaskSubj.get_subscriber().on_next(nullptr);
	});
	///=====================
	/// Camera Data FlowIn
	///====================
	m_Impl->m_CameraDataFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::vector<io::volcap_cam_data_ptr_t> &cameraData) 
	{
		DebugMsg("Got Camera Data");
		/// debug shit
		m_Impl->m_ViewportFrustrumComps.emplace_back(rt::CreateViewportFrustrumComponent());
		rt::MeshMappingSystem::MapViewportFrustrumComp(m_Impl->m_ViewportFrustrumComps.back(), m_Impl->m_ContextComp);
		rt::MeshMappingSystem::AttachViewportCompToToLevelAcceleration(m_Impl->m_ViewportFrustrumComps.back(), m_Impl->m_TopLevelAccelerationComp);
		m_Impl->m_LaunchContextTaskSubj.get_subscriber().on_next(nullptr);
	});
	///=================
	/// Mesh data input
	///=================
	m_Impl->m_MeshDataFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const io::MeshData& data) 
	{
		DebugMsg("Got Mesh Data");
		///
		rt::TriangleMeshComp comp = rt::CreateTriangleMeshComponent();
		rt::MeshMappingSystem::MapMeshDataToTriangleMesh(data, comp, m_Impl->m_ContextComp);
		if (m_Impl->m_TriangleMeshComps.size() == 1)
		{
			rt::MeshMappingSystem::DetachTriangleMeshToTopLevelAcceleration(m_Impl->m_TriangleMeshComps.back(), m_Impl->m_TopLevelAccelerationComp);
			m_Impl->m_TriangleMeshComps.clear();
		}
		rt::MeshMappingSystem::AttachTriangleMeshToAcceleration(comp, m_Impl->m_TopLevelAccelerationComp);
		m_Impl->m_TriangleMeshComps.emplace_back(comp);
		m_Impl->m_LaunchContextTaskSubj.get_subscriber().on_next(nullptr);
	});
	///=================
	/// Camera Rotation
	///=================
	m_Impl->m_CameraRotationTransformFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const mat_t& mat) 
	{
		DebugMsg("Got Camera Rotation");
		optix::Matrix4x4 trmat;
		std::memcpy(trmat.getData(), mat.data(), 16 * sizeof(float));
		rt::RaygenSystem::SetPinholeRaygenTransMat(m_Impl->m_PinholeRaygenComp, trmat);
		m_Impl->m_LaunchContextTaskSubj.get_subscriber().on_next(nullptr);
	});
	///===================
	/// Camera Translation
	///===================
	m_Impl->m_CameraTranslationVectorFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const trans_vec_t& vec) 
	{
		DebugMsg("Got Camera Translation");
		optix::float3 trans;
		trans.x = vec[0];
		trans.y = vec[1];
		trans.z = vec[2];
		rt::RaygenSystem::SetPinholeRaygenEyeTranslation(m_Impl->m_PinholeRaygenComp, trans);
		m_Impl->m_LaunchContextTaskSubj.get_subscriber().on_next(nullptr);
	});
	///====================
	/// Launch Context Task
	///====================
	m_Impl->m_LaunchContextTaskSubj.get_observable().as_dynamic()
		.subscribe([this](auto _) 
	{
		rt::LaunchSystem::Launch(m_Impl->m_ContextComp, m_Impl->m_ViewportSize[0], m_Impl->m_ViewportSize[1], 0);
		rt::LaunchSystem::CopyOutputBuffer(m_Impl->m_PinholeRaygenComp, m_Impl->m_FrameBuffer);
		m_Impl->m_FrameBufferFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameBuffer);
	});
}
/// \brief model destruction
void ViewportTracingModel::Destroy()
{

}

rxcpp::observer<ViewportTracingModel::viewport_size_t> ViewportTracingModel::ViewportSizeFlowIn()
{
	return m_Impl->m_ViewportSizeFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<std::vector<io::volcap_cam_data_ptr_t>> ViewportTracingModel::CameraDataFlowIn()
{
	return m_Impl->m_CameraDataFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<io::MeshData> ViewportTracingModel::MeshDataFlowIn()
{
	return m_Impl->m_MeshDataFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<ViewportTracingModel::mat_t> ViewportTracingModel::CameraRotationTransformFlowIn()
{
	return m_Impl->m_CameraRotationTransformFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<ViewportTracingModel::trans_vec_t> ViewportTracingModel::CameraTranslationVectorFlowIn()
{
	return m_Impl->m_CameraTranslationVectorFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<BufferCPU<uchar4>> ViewportTracingModel::FrameBufferFlowOut()
{
	return m_Impl->m_FrameBufferFlowOutSubj.get_observable().as_dynamic();
}

}	///	!namespace mvt
}	///	!namespace fu