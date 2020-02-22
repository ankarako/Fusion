#ifndef	__VIDEOTRANSCODER_PUBLIC_VIDEOFRAME_H__
#define __VIDEOTRANSCODER_PUBLIC_VIDEOFRAME_H__

#include <Buffer.h>
#include <vector_types.h>

namespace fu {
namespace trans {
///=============
///	Frame Types
///=============
/// We use only standard frames
///	i.e.:
///	= All the underlying frame data are unsigned bytes
///	= The one channel frame is just a grayscale one
///	= The 3 channel frame is an RGB one
///	= The 4 channel frame is an RGBA one
///	\struct VideoFrame
///	\brief video frame data
template <unsigned int NumChannels>
struct VideoFrame
{
	unsigned int Id{ 0u };
};	///	!struct VideoFram
///	\struct Videoframe
///	\brief one channel specialization
///	This is a grayscale frame
template <>
struct VideoFrame<1>
{
	unsigned int Id{ 0u };
	unsigned int Width{ 0u };
	unsigned int Height{ 0u };
	Buffer<float, BufferStorageDevice::CPU> FrameData;
};
///	\struct Videoframe
///	\brief three channel specialization
///	This is an RGB
template <>
struct VideoFrame<3>
{
	unsigned int Id{ 0u };
	unsigned int Width{ 0u };
	unsigned int Height{ 0u };
	Buffer<float3, BufferStorageDevice::CPU> FrameData;
};
///	\struct Videoframe
///	\brief four channel specialization
///	This is an RGBA
template <>
struct VideoFrame<4>
{
	unsigned int Id{ 0u };
	unsigned int Width{ 0u };
	unsigned int Height{ 0u };
	Buffer<float4, BufferStorageDevice::CPU> FrameData;
};
}	///	!namespace trans
}	///	!namespace fu
#endif	///	!__VIDEOTRANSCODER_PUBLIC_VIDEOFRAME_H__