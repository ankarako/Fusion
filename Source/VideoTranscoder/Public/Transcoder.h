#ifndef	__VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__
#define __VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__

#include <Buffer.h>
#include <memory>
#include <string>
#include <vector_types.h>

#include <ffmpegcpp.h>
#include <CodecDeducer.h>

namespace video {


class Transcoder
{
public:
	using vframe_buffer_t = fu::Buffer<float3, fu::BufferStorageProc::CPU>;
	using aframe_buffer_t = fu::Buffer<float, fu::BufferStorageProc::CPU>;
	using debug_frame_buffer_t = fu::Buffer<unsigned char, fu::BufferStorageProc::CPU>;
	Transcoder();
	~Transcoder();
	void InitializeContext();
	void InitializeDecoderContext();
	void InitializeSwScaleContext();
	void LoadFile(const std::string& filepath);
	void DecoderStep();
	void SetVideoScale(uint2 scale);
	/// get video frame
	const vframe_buffer_t& GetCurrentVideoFrame() const;
	///	get audio frame
	const aframe_buffer_t& GetCurrentAudioFrame() const;
	void Destroy();
private:
	int DecodePacket(AVPacket* packet, AVCodecContext* codecCtx, AVFrame* frame);
	void CopyFrameData2VBuffer(AVFrame* frame);
private:
	AVFormatContext*	m_AVFormatContext{ nullptr };
	int					m_StreamCount{ 0 };
	int					m_VideoStreamIndex{ -1 };
	AVCodec*			m_VCodec{ nullptr };
	AVCodecParameters*	m_VCodecParameters{ nullptr };
	AVCodecContext*		m_VCodecContext{ nullptr };
	SwsContext*			m_SwsContext{ nullptr };
	int					m_AudioStreamIndex{ -1 };
	AVCodec*			m_ACodec{ nullptr };
	AVCodecParameters*	m_ACodecParameters{ nullptr };
	AVCodecContext*		m_ACodecContext{ nullptr };

	size_t				m_Duration{ 0 };

	AVFrame*			m_TempVideoFrame{ nullptr };
	AVPacket*			m_TempVideoPacket{ nullptr };
	AVFrame*			m_TempAudioFrame{ nullptr };
	AVPacket*			m_TempAudioPacket{ nullptr };

	vframe_buffer_t		m_CurrentVideoFrame;
	aframe_buffer_t		m_CurrentAudioFrame;
	debug_frame_buffer_t	m_DebugVideoFrame;
	uint2				m_DimScale;
};	///	!class Trancoder
}	///	!namespace video
#endif	///	!__VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__