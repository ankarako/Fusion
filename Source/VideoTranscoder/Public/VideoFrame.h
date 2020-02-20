#ifndef	__VIDEOTRANSCODER_PUBLIC_VIDEOFRAME_H__
#define __VIDEOTRANSCODER_PUBLIC_VIDEOFRAME_H__

namespace video {
///	\struct VideoFrame
///	\brief video frame data
struct VideoFrame
{
	size_t	Id;
	size_t	Width;
	size_t	Height;
	size_t	ChannelCount;
	void*	Format;
	void*	Data;
};	///	!struct VideoFrame
}	///	!namespace video
#endif	///	!__VIDEOTRANSCODER_PUBLIC_VIDEOFRAME_H__