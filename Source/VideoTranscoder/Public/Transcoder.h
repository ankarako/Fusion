#ifndef	__VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__
#define __VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__

#include <memory>
#include <string>

//extern "C" {
//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//}

#include <ffmpegcpp.h>
#include <CodecDeducer.h>

namespace video {


class Transcoder
{
public:
	Transcoder();
	~Transcoder();
	void Initialize();
	void LoadFile(const std::string& filepath);
	void InitializeDecoder();
	void DecoderStep();
	/// get video frame
	///	get audio frame
private:
	int DecodePacket(AVPacket* packet, AVCodecContext* codecCtx, AVFrame* frame);
private:
	AVFormatContext*	m_AVFormatContext{ nullptr };
	int					m_StreamCount;
	int					m_VideoStreamIndex{ -1 };
	AVCodec*			m_VCodec{ nullptr };
	AVCodecParameters*	m_VCodecParameters{ nullptr };
	AVCodecContext*		m_VCodecContext{ nullptr };

	int					m_AudioStreamIndex{ -1 };
	AVCodec*			m_ACodec{ nullptr };
	AVCodecParameters*	m_ACodecParameters{ nullptr };
	AVCodecContext*		m_ACodecContext{ nullptr };

	size_t				m_Duration{ 0 };

	AVFrame*			m_CurrentVideoFrame{ nullptr };
	AVPacket*			m_CurrentVideoPacket{ nullptr };
	AVFrame*			m_CurrentAudioFrame{ nullptr };
	AVPacket*			m_CurrentAudioPacket{ nullptr };
};	///	!class Trancoder
}	///	!namespace video
#endif	///	!__VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__