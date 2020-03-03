#ifndef	__OPTIXRAYTRACER_PUBLIC_SYSTEMS_CREATECONTEXTSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_CREATECONTEXTSYSTEM_H__

#include <Components/ContextComp.h>
#include <optix.h>

namespace fu {
namespace rt {
///	\class OptixCreateContextSystem
///	\brief provides optix context creation functionality
class CreateContextSystem
{
public:
	static void CreateContext(ContextComp ctxComp)
	{
		ctxComp->Context = optix::Context::create();
	}
};	///	!class OptixCreateContextSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_OPTIXCREATECONTEXTSYSTEM_H__