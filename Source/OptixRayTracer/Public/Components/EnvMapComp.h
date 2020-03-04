#ifndef	__OPTIXRAYTRACER_PUBLIC_COMPONENTS_ENVMAPCOMP_H__
#define __OPTIXRAYTRACER_PUBLIC_COMPONENTS_ENVMAPCOMP_H__

#include <optix.h>
#include <optixu/optixpp_namespace.h>
#include <memory>

namespace fu {
namespace rt {
///	\struct EnvMapCompObj
///	\brief holds Environment Map rending data
struct EnvMapCompObj
{
	optix::TextureSampler 	TextureSampler;
	optix::Program			MissProgram;
	optix::Buffer			TextureBuffer;
};	///	!struct EnvMapCompObj
///	\typedef EnvMapComp
///	\brief a reference counted EnvMapCompObj
using EnvMapComp = std::shared_ptr<EnvMapCompObj>;
///	\brief create an EnvMapComp
static EnvMapComp CreateEnvMapComp()
{
	return std::make_shared<EnvMapCompObj>();
}
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_COMPONENTS_ENVMAPCOMP_H__