#ifndef	__VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__
#define __VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__

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
private:
	AVFormatContext* m_FormatContext{ nullptr };
};	///	!class Trancoder
}	///	!namespace video
#endif	///	!__VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__