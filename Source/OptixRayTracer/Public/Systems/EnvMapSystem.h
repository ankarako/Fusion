#ifndef __OPTIXRAYTRACER_PUBLIC_SYSTEMS_ENVMAPSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_ENVMAPSYSTEM_H__

#include <FuAssert.h>
#include <Buffer.h>
#include <Components/ContextComp.h>
#include <Components/EnvMapComp.h>

namespace fu {
namespace rt {
///	\class EnvMapSystem
///	\brief maps environment maps in an optix context
class EnvMapSystem
{
public:
	///	\brief map an envmap component
	///	\param	envComp	the EnvMapComp to map
	///	\param 	ctxComp	the ContextComp	ta map the EnvMapComp
	static void CreateMissProgram(EnvMapComp& envComp, ContextComp& ctxComp)
	{
		/// create the ray tracing program
		envComp->MissProgram = ctxComp->Context->createProgramFromPTXFile(k_EnvMapMissPtxFilepath, k_EnvMapMissProgName);
		ctxComp->Context->setMissProgram(0u, envComp->MissProgram);
	}
	///	\brief create the texture sampler of the envmap component
	///	\param	envComp	the envcironment map component
	///	\param	ctxComp	the optix context component
	static void CreateTexSampler(EnvMapComp& envComp, ContextComp& ctxComp)
	{
		/// create the texture sampler
		envComp->TextureSampler = ctxComp->Context->createTextureSampler();
		/// set wrap modes
		envComp->TextureSampler->setWrapMode(0, RT_WRAP_REPEAT);
		envComp->TextureSampler->setWrapMode(1, RT_WRAP_REPEAT);
		envComp->TextureSampler->setWrapMode(2, RT_WRAP_REPEAT);
		envComp->TextureSampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
		envComp->TextureSampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
		envComp->TextureSampler->setMaxAnisotropy(1.0f);
		envComp->TextureSampler->setMipLevelCount(1u);
		envComp->TextureSampler->setArraySize(1u);
		envComp->TextureSampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_LINEAR);
	}
	///	\brief create the texture buffer
	///	\param	envComp	the environment map component
	///	\param	ctxComp	the context component
	///	\param	width	the buffer's width
	///	\param	height	the buffer's height
	static void CreateBuffer(EnvMapComp& envComp, ContextComp& ctxComp, size_t width, size_t height)
	{
		envComp->TextureBuffer = ctxComp->Context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE4, width, height);
		envComp->TextureSampler->setBuffer(0u, 0u, envComp->TextureBuffer);
		envComp->Width = width;
		envComp->Height = height;
		envComp->MissProgram["EnvMapTextSampler"]->set(envComp->TextureSampler);
	}
	///	\brief set the texture of the texture sampler
	///	\param	envComp	the environment map component
	///	\param	texBuf	the texture buffer on the cpu
	static void SetTexture(EnvMapComp& envComp, BufferCPU<uchar4> texBuf)
	{
		size_t inbsize = texBuf->ByteSize();
		size_t envbsize = envComp->Width * envComp->Height * sizeof(uchar4);
		DebugAssertMsg(inbsize == envbsize, "Failed to copy env map texture to gpu. Buffer sizes are different.");
		std::memcpy(envComp->TextureBuffer->map(), texBuf->Data(), inbsize);
		envComp->TextureBuffer->unmap();
	}
private:
	static constexpr const char* k_EnvMapMissPtxFilepath = "Resources/Programs/EnvMapMiss.ptx";
	static constexpr const char* k_EnvMapMissProgName = "EnvMapMiss";
};	///	!class EnvMapSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_ENVMAPSYSTEM_H__