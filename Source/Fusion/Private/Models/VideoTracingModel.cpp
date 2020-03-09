#include <Models/VideoTracingModel.h>
#include <Components/ContextComp.h>
#include <Components/EnvMapComp.h>
#include <Components/RaygenProgComp.h>
#include <Systems/CreateContextSystem.h>
#include <Systems/EnvMapSystem.h>
#include <Systems/RaygenSystem.h>
#include <Systems/LaunchSystem.h>
#include <GL/gl3w.h>
#include <plog/Log.h>

// debug
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief	VideoTracingModel implementation
struct VideoTracingModel::Impl
{
	BufferCPU<uchar4> m_FrameBuffer;
	///	ray tracing context component
	rt::ContextComp	m_ContextComp;
	///	ray tracing environemnt map component
	///	(a miss program that can render 360 textures)
	rt::EnvMapComp	m_EnvMapComp;
	///	ray tracing ray generation component
	///	can render 360 distorted views
	rt::RaygenProgComp	m_360RaygenComp;
	///	ray tracing ray generation component
	///	can render usual pinhole views
	rt::RaygenProgComp	m_PinholeRaygen;
	///	ray tracing conterxt's launch size
	GLuint				m_PixelBufferHandle;
	GLuint				m_TextureHandle;

	uint2 m_LaunchSize;
	/// frame size flow in
	rxcpp::subjects::subject<uint2>			m_FrameSizeFlowInSubj;
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
	/// frame size flow int task
	/// create the raygen program
	m_Impl->m_FrameSizeFlowInSubj.get_observable()
		.subscribe([this](uint2 size) 
	{
		m_Impl->m_LaunchSize = size;
		//rt::RaygenSystem::Create360RaygenProg(m_Impl->m_360RaygenComp, m_Impl->m_ContextComp, size.x, size.y);
		rt::EnvMapSystem::CreateBuffer(m_Impl->m_EnvMapComp, m_Impl->m_ContextComp, size.x, size.y);
		/// when the frame size is in we have to  create our pixel buffer too
		glGenBuffers(GL_PIXEL_UNPACK_BUFFER, &m_Impl->m_PixelBufferHandle);
		if (m_Impl->m_PixelBufferHandle == 0)
		{
			LOG_ERROR << "Failed to generate pixel buffer.";
		}
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_Impl->m_PixelBufferHandle);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, 
			m_Impl->m_LaunchSize.x * m_Impl->m_LaunchSize.y * sizeof(unsigned char) * 4, nullptr, GL_STREAM_READ);
		/// create our texture handle
		glGenTextures(1, &m_Impl->m_TextureHandle);
		if (m_Impl->m_TextureHandle == 0)
		{
			LOG_ERROR << "Failed to generate texture.";
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Impl->m_TextureHandle);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		/// unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
		/// create output buffer
		rt::RaygenSystem::Create360RaygenProgWithPBO(m_Impl->m_360RaygenComp, m_Impl->m_ContextComp, size, m_Impl->m_PixelBufferHandle);
	});

	m_Impl->m_FrameFlowinSubj.get_observable()
		.subscribe([this](BufferCPU<uchar4>& frame) 
	{
		rt::EnvMapSystem::SetTexture(m_Impl->m_EnvMapComp, frame);
		rt::LaunchSystem::Launch(m_Impl->m_ContextComp, m_Impl->m_LaunchSize.x, m_Impl->m_LaunchSize.y, 0);
		/// copy output buffer
		rt::LaunchSystem::CopyOutputBuffer(m_Impl->m_360RaygenComp, m_Impl->m_FrameBuffer);
		/////////////////
		cv::Mat mat = cv::Mat::zeros(m_Impl->m_LaunchSize.y, m_Impl->m_LaunchSize.x, CV_8UC4);
		cv::imwrite("framemme.png", mat);
		/// send frame to output
		//m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameBuffer);
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
}	///	!namespace fusion
}	///	!namespace fu