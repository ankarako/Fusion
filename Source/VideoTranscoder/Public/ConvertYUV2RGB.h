#ifndef __VIDEOTRANSCODER_PUBLIV_CONVERTYUV2RGB_H__
#define __VIDEOTRANSCODER_PUBLIV_CONVERTYUV2RGB_H__

#include <stdint.h>
#include <vector_types.h>
#include <vector_functions.h>

namespace trans {
///	\brief convert yuv values to rgb
///	\param	y	the y value
///	\param	u	the u value
///	\param	v	the v value
///	\return the converted rgb vector
float3 ConvertYUV2RGB(float y, float u, float v)
{
	float r = y + 1.140f * v;
	float g = y - 0.395f * u - 0.581f * v;
	float b = y + 2.032f * u;
	return make_float3(r, g, b);
}
///	\brief convert yuv to rgb
float3 ConvertYUV8u2RGBf(uint8_t y, uint8_t u, uint8_t v)
{
	/// convert 8-bit values to float
	float yf = (float)y / 255.f;
	float uf = (float)u / 255.f;
	float vf = (float)v / 255.f;
	return ConvertYUV2RGB(yf, uf, vf);
}
///	\brief convert yuv values to rgb
///	\param	y	the y value
///	\param	u	the u value
///	\param	v	the v value
///	\return the converted rgb vector
float4 ConvertYUV2RGBA(float y, float u, float v)
{
	float r = y + 1.140f * v;
	float g = y - 0.395f * u - 0.581f * v;
	float b = y + 2.032f * u;
	float a = 1.0f;
	return make_float4(r, g, b, a);
}
}	///	!namespace trans
#endif	///	!__VIDEOTRANSCODER_PUBLIV_CONVERTYUV2RGB_H__