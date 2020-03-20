#ifndef	__FUSION_PUBLIC_CORE_HELPERS_H__
#define __FUSION_PUBLIC_CORE_HELPERS_H__

#include <Buffer.h>

namespace fu {
namespace fusion {
/// \brief convert a float3 color to a uchar4
///	\param	color	the color to convert
///	\return the converted color
static uchar4 MakeColor(const float3& color)
{
	/// clamp values in [0.0f, 1.0f]
	float x;
	float y;
	float z;
	if (color.x > 1.0f)
	{
		x = 1.0f;
	}
	else if (color.x < 0.0f)
	{
		x = 0.0f;
	}
	else
	{
		x = color.x;
	}
	if (color.y > 1.0f)
	{
		y = 1.0f;
	}
	else if (color.y < 0.0f)
	{
		y = 0.0f;
	}
	else
	{
		y = color.y;
	}
	if (color.z > 1.0f)
	{
		z = 1.0f;
	}
	else if (color.z < 0.0f)
	{
		z = 0.0f;
	}
	else
	{
		z = color.z;
	}

	return make_uchar4(
		static_cast<unsigned char>(x * 255.99f), 
		static_cast<unsigned char>(y * 255.99f),
		static_cast<unsigned char>(z * 255.99f)
		, 255u);
}
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_CORE_HELPERS_H__