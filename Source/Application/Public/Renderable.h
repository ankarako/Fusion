#ifndef __APPLICATION_PUBLIC_RENDERABLE_H__
#define __APPLICATION_PUBLIC_RENDERABLE_H__

namespace app {
///	\class Renderable
///	\brief simple render() interface
class Renderable
{
public:
	virtual void Init() = 0;
	virtual void Render() = 0;
	virtual ~Renderable() = default;
};	///	!class Renderable
}	///	!namespace app
#endif	///	!__APPLICATION_PUBLIC_RENDERABLE_H__