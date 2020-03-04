#ifndef __OPTIXRAYTRACER_PUBLIC_SYSTEMS_ENVMAPSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_ENVMAPSYSTEM_H__

#include <Components/ContextComp.h>
#include <Components/EnvMapComp.h>

namespace fu {
namespace rt {
///	\class EnvMapSystem
///	\brief maps environment maps in an optix context
class EnvMapSystem
{
	///	\brief map an envmap component
	///	\param	envComp	the EnvMapComp to map
	///	\param 	ctxComp	the ContextComp	ta map the EnvMapComp
	static void Map(EnvMapComp& envComp, ContextComp& ctxComp)
	{
		
	}
};	///	!class EnvMapSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_ENVMAPSYSTEM_H__