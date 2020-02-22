#ifndef __COMMON_PUBLIC_DIMS_H__
#define __COMMON_PUBLIC_DIMS_H__

namespace fu {
struct Dims
{
	unsigned int Width{ 0u };
	unsigned int Height{ 0u };
	unsigned int Depth{ 0u };
	///	Cosntruction
	/// \brief Construct from specified dimensions
	///	\param	w	the width component
	///	\param	h	the height component
	///	\param	d	the depth component
	Dims(unsigned int w, unsigned int h, unsigned int d)
		: Width(w), Height(h), Depth(d)
	{ }	
};
}	///	!namespace fu
#endif	///	!__COMMON_PUBLIC_DIMS_H__