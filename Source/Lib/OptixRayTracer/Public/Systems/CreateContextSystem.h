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
	/// \brief Create the ownd context of the specified component
	///	\param	ctxComp	the optix context component to create
	static void CreateContext(ContextComp& ctxComp, unsigned int entryPointCount, unsigned int rayTypeCount)
	{
		ctxComp->Context = optix::Context::create();
		ctxComp->Context->setEntryPointCount(entryPointCount);
		ctxComp->Context->setRayTypeCount(rayTypeCount);
		ctxComp->Context["scene_epsilon"]->setFloat(1.0e-3);
		// TODO: set to false
		ctxComp->Context->setPrintEnabled(true);
	}
	///	\brief destroy an optix context component
	///	Destroys the optix context handle of the specified component
	static void DestroyContext(ContextComp& ctxComp)
	{
		ctxComp->Context->destroy();
	}
};	///	!class OptixCreateContextSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_OPTIXCREATECONTEXTSYSTEM_H__