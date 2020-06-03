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
#include <Systems/AnimationSystem.h>
#include <Programs/RayPayload.cuh>
#include <TextureExportingNode.h>
#include <SavePly.h>
#include <DebugMsg.h>
#include <filesystem>
#include <sstream>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief MVTModel implementation
struct MVTModel::Impl
{
	static constexpr const char* k_CamIdMaskFilename = "mask.png";
	static constexpr const char* k_WeightMaskPrefix = "mask";
	static constexpr const char* k_TextureMapsPrefix = "texels";
	static constexpr const char* k_AnimatedMeshFilename = "animated_template.ply";
	/// ray tracing related
	rt::ContextComp							m_ContextComp;
	rt::AccelerationComp					m_TopLevelAcceleration;
	rt::TriangleMeshComp					m_TriangleMeshComp;
	rt::SolidColorMissComp					m_MissComp;
	std::vector<rt::TexturingCameraComp>	m_TexturingCameraComps;
	uint2									m_ContextLaunchDimensions;
	int										m_ContextLaunchMult{ 1 };
	int										m_CameraCount;
	/// output texture
	uint2									m_TextureSize;
	std::string								m_OutputDir;
	std::string								m_CurrentFrameDir;
	/// running state
	int m_CurrentFrame;
	/// ray tracing output buffers
	std::vector<BufferCPU<uchar4>>			m_TracedColorsBuffers;
	std::vector<BufferCPU<float2>>			m_TracedTexcoordsBuffers;
	std::vector<BufferCPU<float>>			m_TracedWeightsBuffers;
	std::vector<BufferCPU<int>>				m_TracedCamIdBuffers;
	/// texture saving buffers
	std::vector<BufferCPU<uchar4>>			m_OutputTextureColorBuffers;
	std::vector<BufferCPU<uchar4>>			m_OutputTextureWeightBuffers;
	std::vector<BufferCPU<uchar4>>			m_OutputTextureCamIdBuffers;
	/// texture buffer for viewport rendering
	BufferCPU<uchar4>						m_ViewportTextureBuffer;
	
	io::tracked_seq_ptr_t							m_TrackedSequence;
	io::perfcap_skeleton_ptr_t						m_PerfcapSkeleton;
	io::perfcap_skin_data_ptr_t						m_SkinningMatrix;
	io::MeshData									m_TemplateMesh;
	io::MeshData									m_AnimatedMesh;

	bool m_InitializedCameras{ false };
	bool m_InitializedGeometry{ false };
	bool m_SeparateTextures{ true };
	bool m_ViewportEnabled{ false };
	trans::TextureExportingNode						m_TextureExportingNode;

	rxcpp::subjects::subject<io::MeshData>								m_MeshDataFlowInSubj;
	rxcpp::subjects::subject<io::MeshData>								m_MeshDataFlowOutSubj;
	rxcpp::subjects::subject<io::MeshData>								m_AnimatedMeshDataFlowOutSubj;
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
		, m_MissComp(rt::CreateSolidColorMissComponent(optix::make_float3(0.0f, 0.0f, 0.0f)))
		, m_TextureExportingNode(trans::CreateTextureExportingNode())
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
		/// Send camera data to viewport tracing
		m_Impl->m_CameraDataFlowOutSubj.get_subscriber().on_next(cam_data);

		int camCount = cam_data.size();
		m_Impl->m_CameraCount = camCount;
		/// Create the texture tracing context
		rt::CreateContextSystem::CreateContext(m_Impl->m_ContextComp, camCount, 1);
		rt::MissSystem::InitializeSolidColorComp(m_Impl->m_MissComp, m_Impl->m_ContextComp);
		rt::MissSystem::AttachSolidColorMissToContext(m_Impl->m_MissComp, m_Impl->m_ContextComp);
		rt::MeshMappingSystem::NullInitializeAcceleration(m_Impl->m_TopLevelAcceleration, m_Impl->m_ContextComp);
		/// map camera and output buffer data for each camera
		for (int c = 0; c < cam_data.size(); ++c)
		{
			/// create texture camera component
			io::volcap_cam_data_ptr_t cam = cam_data[c];
			rt::TexturingCameraComp comp = rt::CreateTexturingCameraComponent();
			comp->Extrinsics = optix::Matrix4x4(cam->ColorExtrinsics->Data());
			/// was column major
			comp->Extrinsics.transpose();
			optix::Matrix3x3 intr = optix::Matrix3x3(cam->ColorIntrinsics->Data());
			intr.transpose();
			optix::Matrix4x4 intr4;
			intr4.setRow(0, optix::make_float4(intr.getRow(0), 0.0f));
			intr4.setRow(1, optix::make_float4(intr.getRow(1), 0.0f));
			intr4.setRow(2, optix::make_float4(intr.getRow(2), 0.0f));
			intr4.setRow(3, optix::make_float4(0.0f, 0.0f, 0.0f, 0.0f));
			comp->Intrinsics = intr4;
			/// was column major
			comp->Intrinsics;
			m_Impl->m_ContextLaunchDimensions = cam->ColorResolution;
			uint2 launchDims = m_Impl->m_ContextLaunchDimensions * m_Impl->m_ContextLaunchMult;
			rt::RaygenSystem::MapTexturingCamera(comp, m_Impl->m_ContextComp, m_Impl->m_ContextLaunchDimensions, c, m_Impl->m_ContextLaunchMult);
			rt::RaygenSystem::AttachTopLevelAccelerationToTexturingRaygen(comp, m_Impl->m_TopLevelAcceleration);
			m_Impl->m_TexturingCameraComps.emplace_back(comp);

			/// create tracing buffers
			BufferCPU<uchar4> colorBuf = CreateBufferCPU<uchar4>(launchDims.x * launchDims.y);
			m_Impl->m_TracedColorsBuffers.emplace_back(colorBuf);
			BufferCPU<float2> texcoordBuf = CreateBufferCPU<float2>(launchDims.x * launchDims.y);
			m_Impl->m_TracedTexcoordsBuffers.emplace_back(texcoordBuf);
			BufferCPU<float> weightsBuf = CreateBufferCPU<float>(launchDims.x * launchDims.y);
			m_Impl->m_TracedWeightsBuffers.emplace_back(weightsBuf);
			BufferCPU<int> camIdBuf = CreateBufferCPU<int>(launchDims.x * launchDims.y);
			m_Impl->m_TracedCamIdBuffers.emplace_back(camIdBuf);
			/// OutputBuffers for texture atlas
			BufferCPU<uchar4> outColorBuf = CreateBufferCPU<uchar4>(m_Impl->m_TextureSize.x * m_Impl->m_TextureSize.y);
			m_Impl->m_OutputTextureColorBuffers.emplace_back(outColorBuf);
			BufferCPU<uchar4> outTexWeightBuf = CreateBufferCPU<uchar4>(m_Impl->m_TextureSize.x * m_Impl->m_TextureSize.y);
			m_Impl->m_OutputTextureWeightBuffers.emplace_back(outTexWeightBuf);
			BufferCPU<uchar4> outCamIdBuf = CreateBufferCPU<uchar4>(m_Impl->m_TextureSize.x * m_Impl->m_TextureSize.y);
		}
		m_Impl->m_InitializedCameras = true;
		if (m_Impl->m_InitializedCameras && m_Impl->m_InitializedGeometry)
			m_Impl->m_PipelineInitializedSubj.get_subscriber().on_next(nullptr);
	});
	///================
	/// MeshData FlowIn
	///================
	m_Impl->m_MeshDataFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const io::MeshData& data) 
	{
		m_Impl->m_TemplateMesh = data;
		data->TextureWidth = m_Impl->m_TextureSize.x;
		data->TextureHeight = m_Impl->m_TextureSize.y;

		m_Impl->m_ViewportTextureBuffer = CreateBufferCPU<uchar4>(m_Impl->m_TextureSize.x * m_Impl->m_TextureSize.y);
		/// map mesh
		m_Impl->m_MeshDataFlowOutSubj.get_subscriber().on_next(data);
		m_Impl->m_TriangleMeshComp = rt::CreateTriangleMeshComponent();
		rt::MeshMappingSystem::MapMeshDataToPerfcapTriangleMesh(data, m_Impl->m_TriangleMeshComp, m_Impl->m_ContextComp);
		rt::MeshMappingSystem::AttachTriangleMeshToAcceleration(m_Impl->m_TriangleMeshComp, m_Impl->m_TopLevelAcceleration);
		
		m_Impl->m_InitializedGeometry = true;
		if (m_Impl->m_InitializedCameras && m_Impl->m_InitializedGeometry)
			m_Impl->m_PipelineInitializedSubj.get_subscriber().on_next(nullptr);

	}, [this](std::exception_ptr ptr) 
	{
		if (ptr)
		{
			try
			{
				std::rethrow_exception(ptr);
			}
			catch (std::exception& ex)
			{
				LOG_ERROR << ex.what();
			}
		}
	});
	///=================
	/// Skeleton flow in
	///=================
	m_Impl->m_SkeletonFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const io::perfcap_skeleton_ptr_t& skel) 
	{
		m_Impl->m_PerfcapSkeleton = skel;
	});
	///========================
	/// Skinning Matrix flow in
	///========================
	m_Impl->m_SkinningDataFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const io::perfcap_skin_data_ptr_t& skin) 
	{
		m_Impl->m_SkinningMatrix = skin;
	});
	///=================================
	///	Tracked sequence params flow in
	///=================================
	m_Impl->m_TrackedFramesFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const io::tracked_seq_ptr_t& seq)
	{
		m_Impl->m_TrackedSequence = seq;
		m_Impl->m_RunTexturingLoopSubj.get_subscriber().on_next(nullptr);
	});
	///=========================
	/// Camera Textures flow in
	/// Launches texturing pipeline
	///=========================
	m_Impl->m_CameraFramesFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::vector<BufferCPU<uchar4>>& frames) 
	{
		for (int c = 0; c < m_Impl->m_CameraCount; ++c)
		{
			std::memcpy(m_Impl->m_TexturingCameraComps[c]->TextureBuffer->map(), frames[c]->Data(), frames[c]->ByteSize());
			m_Impl->m_TexturingCameraComps[c]->TextureBuffer->unmap();
			rt::LaunchSystem::Launch(m_Impl->m_ContextComp, m_Impl->m_ContextLaunchDimensions.x * m_Impl->m_ContextLaunchMult, m_Impl->m_ContextLaunchDimensions.y * m_Impl->m_ContextLaunchMult, c);
			rt::LaunchSystem::CopyOutputTexturingDataToBuffers(
				m_Impl->m_TexturingCameraComps[c], 
				m_Impl->m_TracedColorsBuffers[c], 
				m_Impl->m_TracedTexcoordsBuffers[c], 
				m_Impl->m_TracedWeightsBuffers[c],
				m_Impl->m_TracedCamIdBuffers[c]);
		}
		DebugMsg("Launched");
		m_Impl->m_RunTextureMergingSubj.get_subscriber().on_next(nullptr);
	});
	///==============
	/// Launch Task
	///==============
	m_Impl->m_RunTexturingLoopSubj.get_observable().as_dynamic()
		.subscribe([this](auto _) 
	{
		using namespace std::experimental;
		/// create directory for saving the output
		if (!filesystem::exists(m_Impl->m_OutputDir))
		{
			filesystem::create_directory(m_Impl->m_OutputDir);
		}
		///===============
		/// Texturing Loop
		///===============
		for (int f = 1; f < m_Impl->m_TrackedSequence->size(); ++f)
		{
			///===========================
			/// create directory for frame
			///===========================
			m_Impl->m_CurrentFrameDir = m_Impl->m_OutputDir + "\\" + std::to_string(f);
			if (!filesystem::exists(m_Impl->m_CurrentFrameDir))
			{
				filesystem::create_directory(m_Impl->m_CurrentFrameDir);
			}
			///=========
			/// Animate
			///=========
			BufferCPU<float> tracked_params = m_Impl->m_TrackedSequence->at(f);
			m_Impl->m_AnimatedMesh = rt::AnimationSystem::AnimateMesh(m_Impl->m_TemplateMesh, m_Impl->m_PerfcapSkeleton, m_Impl->m_SkinningMatrix, tracked_params);
			if (m_Impl->m_ViewportEnabled)
			{
				m_Impl->m_AnimatedMeshDataFlowOutSubj.get_subscriber().on_next(m_Impl->m_AnimatedMesh);
			}
			//Map mesh
			rt::MeshMappingSystem::CopyAnimatedMeshDataToTriangleComp(m_Impl->m_TriangleMeshComp, m_Impl->m_AnimatedMesh);
			//run texturing
			m_Impl->m_CurrentFrame = f;
			m_Impl->m_SeekFrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_CurrentFrame);
		}
		LOG_INFO << "Sequence ended.";
	});
	///======================
	/// Texture Merging Task
	///======================
	m_Impl->m_RunTextureMergingSubj.get_observable().as_dynamic()
		.subscribe([this](auto _) 
	{
		if (m_Impl->m_SeparateTextures)
		{
			// TODO: really tedious for now - reimplement
			for (int c = 0; c < m_Impl->m_CameraCount; ++c)
			{
				BufferCPU<uchar4> colorBuf = m_Impl->m_TracedColorsBuffers[c];
				BufferCPU<float2> texcoordBuf = m_Impl->m_TracedTexcoordsBuffers[c];
				BufferCPU<float> weightsBuf = m_Impl->m_TracedWeightsBuffers[c];

				int count = m_Impl->m_ContextLaunchDimensions.x * m_Impl->m_ContextLaunchDimensions.y * m_Impl->m_ContextLaunchMult;
				
				std::vector<float> weights;
				float minw = 1e16f;
				float maxw = -1e16f;
				// TODO: tediousness for finding min max weights
				for (int p = 0; p < weightsBuf->Count(); ++p)
				{
					float w = weightsBuf->Data()[p];
					weights.emplace_back(w);
					if (weights[p] > maxw)
						maxw = weights[p];
					if (weights[p] < minw)
						minw = weights[p];
				}
				for (int p = 0; p < colorBuf->Count(); ++p)
				{
					float2 texcoord = texcoordBuf->Data()[p];
					texcoord.y = 1.0f - texcoord.y;
					uchar4 color = colorBuf->Data()[p];
					uint2 textureCoord = make_uint2(texcoord.x * m_Impl->m_TextureSize.x, texcoord.y * m_Impl->m_TextureSize.y);
					float weight = (weights[p] - minw) / (maxw - minw) * 255.0f;
					unsigned char nw = (unsigned char)weight;
					uchar4 w = make_uchar4(nw, nw, nw, 255);
					
					int bufcoord = textureCoord.y * m_Impl->m_TextureSize.x + textureCoord.x;
					m_Impl->m_OutputTextureColorBuffers[c]->Data()[bufcoord] = color;
					m_Impl->m_OutputTextureWeightBuffers[c]->Data()[bufcoord] = w;
				}
				std::stringstream ssc;
				ssc << m_Impl->m_CurrentFrameDir << "\\texels-" << std::setw(2) << std::setfill('0') << std::to_string(c);
				std::string filepath = ssc.str();
				m_Impl->m_TextureExportingNode->ExportTexture(filepath + ".png", m_Impl->m_OutputTextureColorBuffers[c], m_Impl->m_TextureSize);
				std::stringstream ssw;
				ssw << m_Impl->m_CurrentFrameDir << "\\mask-" << std::setw(2) << std::setfill('0') << std::to_string(c);
				filepath = ssw.str();
				m_Impl->m_TextureExportingNode->ExportWeights(filepath + ".png", m_Impl->m_OutputTextureWeightBuffers[c], m_Impl->m_TextureSize);
			}
			std::string filepath = m_Impl->m_CurrentFrameDir + "\\" + m_Impl->k_AnimatedMeshFilename;
			io::SavePly(filepath, m_Impl->m_AnimatedMesh);
		}
		else
		{
			for (int c = 0; c < m_Impl->m_CameraCount; ++c)
			{
				BufferCPU<uchar4> colorBuf = m_Impl->m_TracedColorsBuffers[c];
				BufferCPU<float2> texcoordBuf = m_Impl->m_TracedTexcoordsBuffers[c];
				BufferCPU<float> weightBuffer = m_Impl->m_TracedWeightsBuffers[c];

				for (int p = 0; p < colorBuf->Count(); ++p)
				{
					float2 texc = texcoordBuf->Data()[p];
					uchar4 color = colorBuf->Data()[p];
					float weight = weightBuffer->Data()[p];
					// tediousness
					/// convert color to float
					float r = (float)color.x / 255.f * weight;
					float g = (float)color.y / 255.f * weight;
					float b = (float)color.z / 255.f * weight;
					float a = (float)color.w / 255.f;

					float4 colorf = make_float4(r, g, b, a);

					uint2 textureCoord = make_uint2(texc.x * m_Impl->m_TextureSize.x, texc.y * m_Impl->m_TextureSize.y);
					int bufcoord = textureCoord.y * m_Impl->m_TextureSize.x + textureCoord.x;


					m_Impl->m_ViewportTextureBuffer->Data()[bufcoord] = color;
				}
			}
			//m_Impl->m_TextureFlowOutSubj.get_subscriber().on_next(m_Impl->m_ViewportTextureBuffer);
		}
		//============================
		// TODO: delete this once done
		//============================
		if (m_Impl->m_ViewportEnabled)
		{
			for (int c = 0; c < m_Impl->m_CameraCount; ++c)
			{
				BufferCPU<uchar4> buf = m_Impl->m_TracedColorsBuffers[c];
				BufferCPU<float2> texcoord = m_Impl->m_TracedTexcoordsBuffers[c];
				for (int p = 0; p < buf->Count(); ++p)
				{
					float2 texc = texcoord->Data()[p];
					uchar4 color = buf->Data()[p];
					uint2 textureCoord = make_uint2(texc.x * m_Impl->m_TextureSize.x, texc.y * m_Impl->m_TextureSize.y);
					int bufcoord = textureCoord.y * m_Impl->m_TextureSize.x + textureCoord.x;
					m_Impl->m_ViewportTextureBuffer->Data()[bufcoord] = color;
				}
			}
			m_Impl->m_TextureFlowOutSubj.get_subscriber().on_next(m_Impl->m_ViewportTextureBuffer);
		}
	});

}
void MVTModel::SetOutputDir(const std::string & dir)
{
	m_Impl->m_OutputDir = dir;
}
void MVTModel::SetTextureSize(const uint2 & size)
{
	m_Impl->m_TextureSize = size;
}
void MVTModel::SetSeparateTextures(bool set)
{
	m_Impl->m_SeparateTextures = set;
}
void MVTModel::SetLaunchMult(int mult)
{
	m_Impl->m_ContextLaunchMult = mult;
}
void MVTModel::SetViewportEnabled(bool enabled)
{
	m_Impl->m_ViewportEnabled = enabled;
}
rxcpp::observer<io::MeshData> MVTModel::MeshDataFlowIn()
{
	return m_Impl->m_MeshDataFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observable<io::MeshData> MVTModel::MeshDataFlowOut()
{
	return m_Impl->m_MeshDataFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<io::MeshData> MVTModel::AnimatedMeshDataFlowOut()
{
	return m_Impl->m_AnimatedMeshDataFlowOutSubj.get_observable().as_dynamic();
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