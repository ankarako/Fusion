#ifndef __VIDEOTRANSCODER_PUBLIC_STREAM_H__
#define __VIDEOTRANSCODER_PUBLIC_STREAM_H__

#include <ffmpegcpp.h>
#include <map>
#include <string>

namespace fu {
namespace trans {
///	\enum StreamType
///	\brief the type of a stream
enum class StreamType {
	Unknown,
	Video, 
	Audio,
	Subtitle,
};	///	!enum StreamType
///	convert stream type to string
static const std::map<StreamType, std::string> StreamType2Str =
{
	{ StreamType::Video, "Video" },
	{ StreamType::Audio, "Audio" },
	{ StreamType::Subtitle, "Subtitle" }
};
///	\struct Stream
///	\brief a POD holding stream related data
struct Stream
{
	unsigned int		Id{ 0 };
	AVCodecContext*		CodecContext{ nullptr };
	AVCodecParameters*	CodecParams{ nullptr };
	AVCodec*			Codec{ nullptr };
	AVPacket*			Packet{ nullptr };
	AVFrame*			Frame{ nullptr };
};
}	///	!namespace trans
}	///	!namesapce fu
#endif	///	!__VIDEOTRANSCODER_PUBLIC_STREAM_H__