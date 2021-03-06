#ifndef	__OPTIXRAYTRACER_PUBLIC_SYSTEMS_LAUNCHSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_LAUNCHSYSTEM_H__

#include <Buffer.h>
#include <Components/ContextComp.h>
#include <Components/RaygenProgComp.h>
#include <Components/TexturingCameraComp.h>
#include <optix_world.h>
#include <plog/Log.h>
#include <Programs/RayPayload.cuh>

namespace fu {
namespace rt {
///	\class LaunchSystem
///	\brief laucnhes optix contexts
class LaunchSystem
{
public:
	///	\brief launch a context
	///	\param	ctxComp	the context component to launch
	///	\param	width	the width of the launch
	///	\param	height 	the height of the launch
	///	\param	entry	the entry point idx of the launch
	static void Launch(ContextComp& ctxComp, int width, int height, int entry)
	{
		try
		{
			ctxComp->Context->validate();
			ctxComp->Context->launch(entry, width, height);
		}
		catch (optix::Exception& ex)
		{
			LOG_ERROR << "Failed to launch context. Error: " << ex.getErrorString();
		}
	}
	/// \brief copy the context's output buffer data into another one
	///	\param	raygenComp	the raygen component to copy its buffer
	///	\param	outBuffer	the buffer to copy the data into
	static void CopyOutputBuffer(RaygenProgComp& rayComp, BufferCPU<uchar4>& outBuffer)
	{
		std::memcpy(outBuffer->Data(), rayComp->OutputBuffer->map(), outBuffer->ByteSize());
		rayComp->OutputBuffer->unmap();
	}

	static void CopyOutputTexturingBuffer(TexturingCameraComp& camComp, BufferCPU<TexturingOutput>& outBuffer)
	{
		std::memcpy(outBuffer->Data(), camComp->TexturingOutputBuffer->map(), outBuffer->ByteSize());
		camComp->TexturingOutputBuffer->unmap();
	}

	static void CopyOutputTexturingDataToBuffers(TexturingCameraComp& camComp, 
		BufferCPU<uchar4>& texBuf, 
		BufferCPU<float2>& texcoordBuf, 
		BufferCPU<float>& weightBuf,
		BufferCPU<int>& camIdBuf)
	{
		std::memcpy(texBuf->Data(), camComp->OutputColorBuffer->map(), texBuf->ByteSize());
		camComp->OutputColorBuffer->unmap();
		std::memcpy(texcoordBuf->Data(), camComp->OutputTexcoordBuffer->map(), texcoordBuf->ByteSize());
		camComp->OutputTexcoordBuffer->unmap();
		std::memcpy(weightBuf->Data(), camComp->OutputWeightBuffer->map(), weightBuf->ByteSize());
		camComp->OutputWeightBuffer->unmap();
		std::memcpy(camIdBuf->Data(), camComp->OutputCamIdBuffer->map(), camIdBuf->ByteSize());
		camComp->OutputCamIdBuffer->unmap();
	}
};	///	!class LaunchSystem
}	///	!namespace rt
}	///	!namespace fu
#endif 	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_LAUNCHSYSTEM_H__