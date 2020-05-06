#ifndef	__OPTIXRAYTRACER_PUBLIC_SYSTEMS_MISSSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_MISSSYSTEM_H__

#include <Components/ContextComp.h>
#include <Components/SolidColorMissComp.h>

namespace fu {
namespace rt {
///	\class MissSystem
///	\brief maps miss programs
class MissSystem
{
public:
	///	\brief create a solir color miss component
	///	\param	the component to create
	static void InitializeSolidColorComp(SolidColorMissComp& missComp, ContextComp& ctxComp)
	{
		missComp->MissProg = ctxComp->Context->createProgramFromPTXFile(k_SolidColorMissFilepath, k_SolidColorMissProgName);
		missComp->MissProg["solid_color"]->setFloat(missComp->SolidColor);
	}
	/// \brief attach miss program to context
	static void AttachSolidColorMissToContext(SolidColorMissComp& missComp, ContextComp& ctxComp)
	{
		ctxComp->Context->setMissProgram(0u, missComp->MissProg);
	}
private:
	static constexpr const char* k_SolidColorMissFilepath = "FusionLib/Resources/Programs/SolidColorMiss.ptx";
	static constexpr const char* k_SolidColorMissProgName = "SolidColorMiss";
};	///	!class MissSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_MISSSYSTEM_H__