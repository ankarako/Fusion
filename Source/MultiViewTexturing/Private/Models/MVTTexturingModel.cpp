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
#include <Programs/RayPayload.cuh>
#include <DebugMsg.h>

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
	uint2									m_TextureSize;

	BufferCPU<uchar4>						m_OutputTextureBuffer;
	BufferCPU<float2>						m_OutputTexcoordBuffer;
	BufferCPU<int>							m_OutputCamIdBuffer;
	BufferCPU<float>						m_OutputDistBuffer;
	int										m_CameraCount;
	std::vector<BufferCPU<rt::TexturingOutput>>		m_TexturingOutputBuffers;
	bool m_InitializedCameras{ false };
	bool m_InitializedGemetry{ false };
	io::tracked_seq_ptr_t						m_TrackedSequence;

	rxcpp::subjects::subject<io::MeshData>								m_MeshDataFlowInSubj;
	rxcpp::subjects::subject<io::MeshData>								m_MeshDataFlowOutSubj;
	rxcpp::subjects::subject<std::vector<io::volcap_cam_data_ptr_t>>	m_CameraDataFlowInSubj;
	rxcpp::subjects::subject<std::vector<io::volcap_cam_data_ptr_t>>	m_CameraDataFlowOutSubj;
	rxcpp::subjects::subject<std::vector<BufferCPU<uchar4>>>			m_CameraFramesFlowInSubj;
	rxcpp::subjects::subject<io::tracked_seq_ptr_t>						m_TrackedFramesFlowInSubj;
	rxcpp::subjects::subject<io::perfcap_skeleton_ptr_t>				m_SkeletonFlowInSubj;
	rxcpp::subjects::subject<io::perfcap_skin_data_ptr_t>				m_SkinningDataFlowInSubj;
	rxcpp::subjects::subject<void*>										m_RunTexturingLoopSubj;
	rxcpp::subjects::subject<int>										m_SeekFrameFlowOutSubj;
	rxcpp::subjects::subject<void*>										m_PipelineInitializedSubj;
	rxcpp::subjects::subject<void*>										m_RunTextureMergingSubj;
	rxcpp::subjects::subject<BufferCPU<uchar4>>							m_TextureFlowOutSubj;
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
		m_Impl->m_CameraDataFlowOutSubj.get_subscriber().on_next(cam_data);
		int camCount = cam_data.size();
		m_Impl->m_CameraCount = camCount;
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
			optix::uint2 size = cam->ColorResolution;
			rt::RaygenSystem::MapTexturingCamera(comp, m_Impl->m_ContextComp, size, c);
			rt::RaygenSystem::AttachTopLevelAccelerationToTexturingRaygen(comp, m_Impl->m_TopLevelAcceleration);
			m_Impl->m_TexturingCameraComps.emplace_back(comp);
			BufferCPU<rt::TexturingOutput> buf = CreateBufferCPU<rt::TexturingOutput>(cam->ColorResolution.x * cam->ColorResolution.y);
			m_Impl->m_TexturingOutputBuffers.emplace_back(buf);
		}
		m_Impl->m_InitializedCameras = true;
		if (m_Impl->m_InitializedCameras && m_Impl->m_InitializedGemetry)
			m_Impl->m_PipelineInitializedSubj.get_subscriber().on_next(nullptr);
	});
	///================
	/// MeshData FlowIn
	///================
	m_Impl->m_MeshDataFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const io::MeshData& data) 
	{
		m_Impl->m_MeshDataFlowOutSubj.get_subscriber().on_next(data);
		m_Impl->m_OutputTextureBuffer = CreateBufferCPU<uchar4>(data->TextureWidth * data->TextureHeight);
		m_Impl->m_TextureSize = make_uint2(data->TextureWidth, data->TextureHeight);
		m_Impl->m_TriangleMeshComp = rt::CreateTriangleMeshComponent();
		rt::MeshMappingSystem::MapMeshDataToTexturingTriangleMesh(data, m_Impl->m_TriangleMeshComp, m_Impl->m_ContextComp);
		//rt::MeshMappingSystem::DetachTriangleMeshToTopLevelAcceleration()
		rt::MeshMappingSystem::AttachTriangleMeshToAcceleration(m_Impl->m_TriangleMeshComp, m_Impl->m_TopLevelAcceleration);
		m_Impl->m_InitializedGemetry = true;
		if (m_Impl->m_InitializedCameras && m_Impl->m_InitializedGemetry)
			m_Impl->m_PipelineInitializedSubj.get_subscriber().on_next(nullptr);
		//m_Impl->m_RunTexturingLoopSubj.get_subscriber().on_next(nullptr);
	});

	m_Impl->m_TrackedFramesFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const io::tracked_seq_ptr_t& seq)
	{
		m_Impl->m_TrackedSequence = seq;
		m_Impl->m_RunTexturingLoopSubj.get_subscriber().on_next(nullptr);
	});

	m_Impl->m_CameraFramesFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::vector<BufferCPU<uchar4>>& frames) 
	{
		for (int c = 0; c < m_Impl->m_CameraCount; ++c)
		{
			std::memcpy(m_Impl->m_TexturingCameraComps[c]->TextureBuffer->map(), frames[c]->Data(), frames[c]->ByteSize());
			m_Impl->m_TexturingCameraComps[c]->TextureBuffer->unmap();
			rt::LaunchSystem::Launch(m_Impl->m_ContextComp, m_Impl->m_TexturingCameraComps[c]->CamPlaneWidth, m_Impl->m_TexturingCameraComps[c]->CamPlaneHeight, c);
			rt::LaunchSystem::CopyOutputTexturingBuffer(m_Impl->m_TexturingCameraComps[c], m_Impl->m_TexturingOutputBuffers[c]);
			DebugMsg("Ran one Launch");
		}
		m_Impl->m_RunTextureMergingSubj.get_subscriber().on_next(nullptr);
	});
	///==============
	/// Launch Task
	///==============
	m_Impl->m_RunTexturingLoopSubj.get_observable().as_dynamic()
		.subscribe([this](auto _) 
	{
		/*for (int f = 0; f < m_Impl->m_TrackedSequence->size(); ++f)
		{*/
			m_Impl->m_SeekFrameFlowOutSubj.get_subscriber().on_next(0);
		//}
	});
	///======================
	/// Texture Merging Task
	///======================
	m_Impl->m_RunTextureMergingSubj.get_observable().as_dynamic()
		.subscribe([this](auto _) 
	{
		for (int c = 0; c < m_Impl->m_TexturingOutputBuffers.size(); ++c)
		{
			BufferCPU<rt::TexturingOutput> buf = m_Impl->m_TexturingOutputBuffers[c];
			for (int p = 0; p < buf->Count(); ++p)
			{
				float2 texcoord = buf->Data()[p].Texcoord;
				uchar4 color = buf->Data()[p].Color;
				uint2 textureCoord = make_uint2(texcoord.x * m_Impl->m_TextureSize.x, texcoord.y * m_Impl->m_TextureSize.y);
				int bufcoord = textureCoord.y * m_Impl->m_TextureSize.x + textureCoord.x;
				m_Impl->m_OutputTextureBuffer->Data()[bufcoord] = color;
			}
		}
		m_Impl->m_TextureFlowOutSubj.get_subscriber().on_next(m_Impl->m_OutputTextureBuffer);
	});

}
rxcpp::observer<io::MeshData> MVTModel::MeshDataFlowIn()
{
	return m_Impl->m_MeshDataFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observable<io::MeshData> MVTModel::MeshDataFlowOut()
{
	return m_Impl->m_MeshDataFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observer<std::vector<io::volcap_cam_data_ptr_t>> MVTModel::CameraDataFlowIn()
{
	return m_Impl->m_CameraDataFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observable<std::vector<io::volcap_cam_data_ptr_t>> MVTModel::CameraDataFlowOut()
{
	return m_Impl->m_CameraDataFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observer<std::vector<BufferCPU<uchar4>>> MVTModel::CameraFramesFlowIn()
{
	return m_Impl->m_CameraFramesFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<io::tracked_seq_ptr_t> MVTModel::TrackedFramesFlowIn()
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

rxcpp::observable<BufferCPU<uchar4>> MVTModel::TextureFlowOut()
{
	return m_Impl->m_TextureFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace mvt
}	///	!namespace fu