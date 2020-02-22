#ifndef	__FUSION_PUBLIC_CORE_CAMERA_H__
#define __FUSION_PUBLIC_CORE_CAMERA_H__

#include <optix.h>
#include <optixu/optixpp_namespace.h>

namespace fu {
namespace fusion {

enum class CameraShaderType
{
	Unknown,
	Pinhole,
	Omnidirectional
};

class Camera 
{ 
public:
	static void Create(CameraShaderType type, optix::float3 eye, optix::float3 lookat, optix::float3 up);
	
private:
	optix::float3 m_Eye;
	optix::flaot3 m_Lookat;
	optix::float3 m_Up;
};


}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_CORE_CAMERA_H__