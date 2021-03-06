#ifndef	__APPLICATION_PUBLIC_INITIALIZABLE_H__
#define __APPLICATION_PUBLIC_INITIALIZABLE_H__

namespace fu {
namespace app {
///	\class initializable
///	\brief a simple interface that provides initialization
class Initializable
{
public:
	virtual void Init() = 0;
	virtual ~Initializable() = default;
};	///	!class Initializable
}	///	!namespace app
} /// !namespace fu
#endif	///	!__APPLICATION_PUBLIC_INITIALIZABLE_H__