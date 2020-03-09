#include <Models/VideoTracingModel.h>
#include <Components/ContextComp.h>
#include <Components/EnvMapComp.h>
#include <Components/RaygenProgComp.h>
#include <Components/TriangleMeshComp.h>
#include <Components/AccelerationComp.h>
#include <Systems/CreateContextSystem.h>
#include <Systems/EnvMapSystem.h>
#include <Systems/RaygenSystem.h>
#include <Systems/LaunchSystem.h>
#include <Systems/MeshMappingSystem.h>
#include <GL/gl3w.h>
#include <plog/Log.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>

#include <vector>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief	VideoTracingModel implementation
struct VideoTracingModel::Impl
{
	BufferCPU<uchar4> m_FrameBuffer;
	///	ray tracing context component
	rt::ContextComp			m_ContextComp;
	///	ray tracing environemnt map component
	///	(a miss program that can render 360 textures)
	rt::EnvMapComp			m_EnvMapComp;
	///	ray tracing ray generation component
	///	can render 360 distorted views
	rt::RaygenProgComp		m_360RaygenComp;
	///	ray tracing ray generation component
	///	can render usual pinhole views
	rt::RaygenProgComp		m_PinholeRaygenComp;
	/// mesh instance component
	rt::AccelerationComp	m_AccelrationComp;
	///	triangle meshes
	std::vector<rt::TriangleMeshComp> m_TriangleMeshComps;
	/// the context launch size
	uint2 m_LaunchSize;
	/// frame size flow in
	rxcpp::subjects::subject<uint2>			m_FrameSizeFlowInSubj;
	/// gl pixel buffer flow in
	rxcpp::subjects::subject<GLuint>		m_PboFlowInSubj;
	///frame output
	rxcpp::subjects::subject<output_frame_t>	m_FrameFlowOutSubj;
	///	frame input
	rxcpp::subjects::subject<input_frame_t>		m_FrameFlowinSubj;
	/// Construction
	Impl() { }
};	///	!struct Impl
///	Construction
///	\brief initializes the model
VideoTracingModel::VideoTracingModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
///	\brief initialize the model
///	mostly assigns tasks to subscriptions
void VideoTracingModel::Init()
{
	/// initialize the ray tracing context component
	m_Impl->m_ContextComp = rt::CreateContextComponent();
	/// create the ray tracing context
	rt::CreateContextSystem::CreateContext(m_Impl->m_ContextComp, 1, 1);
	/// initialize the env map component
	m_Impl->m_EnvMapComp = rt::CreateEnvMapComp();
	/// Create the program of the envmap component
	rt::EnvMapSystem::CreateMissProgram(m_Impl->m_EnvMapComp, m_Impl->m_ContextComp);
	/// Create the texture sampler
	rt::EnvMapSystem::CreateTexSampler(m_Impl->m_EnvMapComp, m_Impl->m_ContextComp);
	/// create the ray generation component
	m_Impl->m_360RaygenComp = rt::CreateRaygenProgComponent();
	/// create a dummy first triangle mesh component
	m_Impl->m_TriangleMeshComps.emplace_back(rt::CreateTriangleMeshComponent());
	/// intialize the triangle mesh component with no data
	rt::MeshMappingSystem::NullInitializeTriangleMesh(m_Impl->m_TriangleMeshComps.back(), m_Impl->m_ContextComp);
	/// null initialize the acceleration comp
	m_Impl->m_AccelrationComp = rt::CreateAccelerationComponent();
	/// initialize the mesh instance component
	rt::MeshMappingSystem::NullInitializeAcceleration(m_Impl->m_AccelrationComp, m_Impl->m_ContextComp);
	/// attach triangle mesh component to acceleration
	rt::MeshMappingSystem::AttachTriangleMeshToAcceleration(m_Impl->m_TriangleMeshComps.back(), m_Impl->m_AccelrationComp);
	/// frame size flow int task
	/// create the raygen program
	m_Impl->m_FrameSizeFlowInSubj.get_observable()
		.subscribe([this](uint2 size) 
	{
		m_Impl->m_LaunchSize = size;
		rt::RaygenSystem::Create360RaygenProg(m_Impl->m_360RaygenComp, m_Impl->m_ContextComp, size.x, size.y);
		rt::RaygenSystem::SetRaygenAttributes(m_Impl->m_360RaygenComp);
		rt::EnvMapSystem::CreateTextureBuffer(m_Impl->m_EnvMapComp, m_Impl->m_ContextComp, size.x, size.y);
		rt::MeshMappingSystem::MapAccelerationToRaygen(m_Impl->m_AccelrationComp, m_Impl->m_360RaygenComp);
		/// create our frame buffer
		m_Impl->m_FrameBuffer = CreateBufferCPU<uchar4>(size.x * size.y);
	});

	m_Impl->m_PboFlowInSubj.get_observable()
		.subscribe([this](GLuint pbohandle) 
	{
		//rt::RaygenSystem::Create360RaygenProgWithPBO(m_Impl->m_360RaygenComp, m_Impl->m_ContextComp, m_Impl->m_LaunchSize, pbohandle);
	});
	

	m_Impl->m_FrameFlowinSubj.get_observable()
		.subscribe([this](BufferCPU<uchar4>& frame) 
	{
		rt::EnvMapSystem::SetTexture(m_Impl->m_EnvMapComp, frame);
		rt::LaunchSystem::Launch(m_Impl->m_ContextComp, m_Impl->m_LaunchSize.x, m_Impl->m_LaunchSize.y, 0);
		/// copy output buffer
		rt::LaunchSystem::CopyOutputBuffer(m_Impl->m_360RaygenComp, m_Impl->m_FrameBuffer);
		/// send frame to output
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameBuffer);
	});
}
/// \brief destroy the model
///	destroys whatever needs to be destroyed
void VideoTracingModel::Destroy()
{
	/// destroy the ray tracing context component
	rt::CreateContextSystem::DestroyContext(m_Impl->m_ContextComp);
}
rxcpp::observer<uint2> VideoTracingModel::FrameSizeFlowIn()
{
	return m_Impl->m_FrameSizeFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
/// frame output
rxcpp::observable<VideoTracingModel::output_frame_t> fu::fusion::VideoTracingModel::FrameFlowOut()
{
	return m_Impl->m_FrameFlowOutSubj.get_observable().as_dynamic();
}
/// frame input
rxcpp::observer<VideoTracingModel::input_frame_t> fu::fusion::VideoTracingModel::FrameFlowIn()
{
	return m_Impl->m_FrameFlowinSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<GLuint> fu::fusion::VideoTracingModel::PboFlowIn()
{
	return m_Impl->m_PboFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu