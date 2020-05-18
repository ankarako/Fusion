#ifndef __MVT_PUBLIC_CORE_EXTERNALAPP_H__
#define __MVT_PUBLIC_CORE_EXTERNALAPP_H__

#include <string>

namespace fu {
namespace mvt {
///	\struct ExternalApp
///	\brief a simple data struct for running external applications
struct ExternalApp
{
	std::string ExePath;
	std::string	Cli;
	/// \brief run the specified executable with the specified cli
	int Run()
	{
		std::string cli = ExePath + " " + Cli;
		int ret = std::system(cli.c_str());
		return ret;
	}
};	///	!struct ExternalApp
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_CORE_EXTERNALAPP_H__