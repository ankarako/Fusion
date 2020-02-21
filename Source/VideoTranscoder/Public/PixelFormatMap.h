#ifndef	__VIDEOTRANSCODER_PUBLIC_PIXELFORMATMAP_H__
#define __VIDEOTRANSCODER_PUBLIC_PIXELFORMATMAP_H__

#include <ffmpegcpp.h>
#include <string>
#include <map>

namespace video {
///	\brief a simple map that maps ffmpeg pixel formats to string
static const std::map<int, std::string> PixelFormat2Str = 
{
	{ AVPixelFormat::AV_PIX_FMT_YUV420P,	"YUV420P" },
	{ AVPixelFormat::AV_PIX_FMT_ARGB,		"ARGB" },
	{ AVPixelFormat::AV_PIX_FMT_ABGR,		"ABGR" },
	{ AVPixelFormat::AV_PIX_FMT_0RGB,		"0RGB" },
	{ AVPixelFormat::AV_PIX_FMT_0BGR,		"0BGR" },
};
}	///	!namespace video
#endif	///	!__VIDEOTRANSCODER_PUBLIC_PIXELFORMATMAP_H__