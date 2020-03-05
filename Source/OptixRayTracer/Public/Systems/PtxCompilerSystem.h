#ifndef __OPTIXRAYTRACER_PUBLIC_PTXCOMPILERSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_PTXCOMPILERSYSTEM_H__

#include <FuAssert.h>
#include <nvrtc.h>
#include <fstream>
#include <sstream>
#include <string>

namespace fu {
namespace rt {
///	\class PtxCompilerSystem
///	\brief ptx compiling functionalities utilizing nvrtc (NVIDIA Runtime Compiler)
class PtxCompilerSystem
{
public:
	///	\brief compile a cuda source file
	///	\param	filepath	the path of the cuda source file
	///	\return the compiled ptx code as a string
	static std::string CompileCuSource(const std::string& filepath)
	{
		HardAssertMsg(false, "Not implemented");
	}
	///	\brief read a source file
	///	\param	filepath	the specified source file's path
	///	\return the read source as a string
	static std::string ReadSrcFile(const std::string& filepath)
	{
		std::string src;
		std::ifstream inFd(filepath);
		if (file.good())
		{
			std::stringstream fileBuf;
			fileBuf << file.rdbuf();
			src = fileBuf.str();
		}
		return src;
	}
private:
	/// flags
	/// include paths
	static constexpr const char* k_CUDAIncludeDir = "C:/Program Files/NVIDIA GPU ComputingToolkit/CUDA/v10.2/include";
	// static constexpr const char* k_OptixIncludeDir = ""
	static constexpr const char* k_IncludeDir = "Resources/Programs";
	static constexpr const char* k_UseFastMathFlag = "--use-fast-math";
};	///	!class PtxCompilerSyste
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_PTXCOMPILERSYSTEM_H__