#ifndef	__VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__
#define __VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__

#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace video {
class Transcoder
{
public:
	Transcoder();
	void Initialize();
	void LoadFile(const std::string& filepath);
private:
	AVFormatContext* m_FormatContext{ nullptr };
};	///	!class Trancoder
}	///	!namespace video
#endif	///	!__VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__