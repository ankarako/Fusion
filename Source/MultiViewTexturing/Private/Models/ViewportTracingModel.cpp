#include <Models/ViewportTracingModel.h>
#include <Components/AccelerationComp.h>
#include <Components/RaygenProgComp.h>
#include <Components/SolidColorMissComp.h>
#include <Components/TriangleMeshComp.h>
#include <Components/PointCloudComp.h>
#include <Components/ContextComp.h>
#include <Components/ViewportFrustrumComp.h>
#include <Systems/RaygenSystem.h>
#include <Systems/MissSystem.h>
#include <Systems/CreateContextSystem.h>
#include <Systems/MeshMappingSystem.h>
#include <Systems/LaunchSystem.h>
//#include <Systems/ObjectMappingSystem.h>

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
	rt::PointCloudComp						m_PointCloudComp;
	std::vector<rt::ViewportFrustrumComp>	m_ViewportFrustrumComps;
	/// IO
	rxcpp::subjects::subject<viewport_size_t>							m_ViewportSizeFlowInSubj;
	rxcpp::subjects::subject<std::vector<io::volcap_cam_data_ptr_t>>	m_CameraDataFlowInSubj;
	rxcpp::subjects::subject<std::vector<BufferCPU<uchar4>>>			m_VideoTexturesFlowInSubj;
	rxcpp::subjects::subject<io::MeshData>								m_MeshDataFlowInSubj;
	rxcpp::subjects::subject<mat_t>										m_CameraRotationTransformFlowInSubj;
	rxcpp::subjects::subject<trans_vec_t>								m_CameraTranslationVectorFlowInSubj;
	rxcpp::subjects::subject<void*>										m_LeftMouseButtonStartTrackingSubj;
	rxcpp::subjects::subject<void*>										m_LeftMouseButtonStopTrackingSubj;
	rxcpp::subjects::subject<void*>										m_RightMouseButtonStartTrackingSubj;
	rxcpp::subjects::subject<void*>										m_RightMouseButtonStopTrackingSubj;
	rxcpp::subjects::subject<mouse_pos_t>								m_LeftMouseButtonPosFlowInSubj;
	rxcpp::subjects::subject<mouse_pos_t>								m_rightMouseButtonPosFlowInSubj;
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
	rt::RaygenSystem::CreatePinholeRaygenProg(m_Impl->m_PinholeRaygenComp, m_Impl->m_ContextComp, 1000, 500);
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
		m_Impl->m_PointCloudComp = rt::CreatePointCloudComponent();
		io::MeshData meshData = io::CreateMeshData();
		meshData->VertexBuffer = CreateBufferCPU<float3>(cameraData.size());
		meshData->ColorBuffer = CreateBufferCPU<uchar4>(cameraData.size());
		meshData->HasColors = true;
		meshData->HasNormals = false;
		meshData->HasFaces = false;
		meshData->HasTexcoords = false;
		for (int c = 0; c < cameraData.size(); ++c)
		{
			optix::float3 translation;
			float rotation[9];
			BufferCPU<float> depthExt = cameraData[c]->DepthExtrinsics;
			translation.x = depthExt->Data()[3];
			translation.y = depthExt->Data()[7];
			translation.z = depthExt->Data()[11];
			rotation[0] = depthExt->Data()[0];
			rotation[1] = depthExt->Data()[1];
			rotation[2] = depthExt->Data()[2];
			rotation[3] = depthExt->Data()[4];
			rotation[4] = depthExt->Data()[5];
			rotation[5] = depthExt->Data()[6];
			rotation[6] = depthExt->Data()[8];
			rotation[7] = depthExt->Data()[9];
			rotation[8] = depthExt->Data()[10];
			optix::Matrix3x3 rotMat = optix::Matrix3x3(rotation);

			uchar4 color = make_uchar4(20 * (c + 1), 10 * (c + 1), 30 * (c + 1), 255);
			meshData->VertexBuffer->Data()[c] = translation;
			meshData->ColorBuffer->Data()[c] = color;
			/// get translation vector
		}
		rt::MeshMappingSystem::MapMeshDataToPointCloud(meshData, m_Impl->m_PointCloudComp, m_Impl->m_ContextComp);
		rt::MeshMappingSystem::SetPointcloudCompPointSize(m_Impl->m_PointCloudComp, 0.05f);
		rt::MeshMappingSystem::AttachPointCloudCompToTopLevelAcceleration(m_Impl->m_PointCloudComp, m_Impl->m_TopLevelAccelerationComp);
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
	/// Camera manipulation
	///====================

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
	rt::CreateContextSystem::DestroyContext(m_Impl->m_ContextComp);
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

rxcpp::observer<void*> ViewportTracingModel::LeftMouseButtonStartTracking()
{
	return m_Impl->m_LeftMouseButtonStartTrackingSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<void*> ViewportTracingModel::LeftMouseButtonStopTracking()
{
	return m_Impl->m_LeftMouseButtonStopTrackingSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<void*> ViewportTracingModel::RightMouseButtonStartTracking()
{
	return m_Impl->m_RightMouseButtonStartTrackingSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<void*> ViewportTracingModel::RightMouseButtonStopTracking()
{
	return m_Impl->m_RightMouseButtonStopTrackingSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<ViewportTracingModel::mouse_pos_t> ViewportTracingModel::LeftMouseButtonPosFlowIn()
{
	return m_Impl->m_LeftMouseButtonPosFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<ViewportTracingModel::mouse_pos_t> ViewportTracingModel::RightMouseButtonPosFlowIn()
{
	return m_Impl->m_rightMouseButtonPosFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<BufferCPU<uchar4>> ViewportTracingModel::FrameBufferFlowOut()
{
	return m_Impl->m_FrameBufferFlowOutSubj.get_observable().as_dynamic();
}

}	///	!namespace mvt
}	///	!namespace fu