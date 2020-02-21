#include <Transcoder.h>
#include <plog/Log.h>
#include <PixelFormatMap.h>
#include <ConvertYUV2RGB.h>
#include <vector_types.h>
#include <vector_functions.h>

namespace video {
///	Construction
///	\brief default constructor
Transcoder::Transcoder() { }
/// Destruction
Transcoder::~Transcoder()
{
	//Destroy();
}
///	\brief Initialize the transcoding context
///	Allocates an av format context
void Transcoder::InitializeContext()
{ 
	/// Initialize the format context
	m_AVFormatContext = avformat_alloc_context();
	if (!m_AVFormatContext)
		throw std::exception("Failed to allocate AV Context.");
	// TODO: change this to quiet when everything works
#if defined(_DEBUG) || defined(NDEBUG)
	av_log_set_level(AV_LOG_DEBUG);
#else
	av_log_set_level(AV_LOG_ERROR);
#endif
}
///
void Transcoder::LoadFile(const std::string& filepath)
{
	/// Open the input file
	LOG_INFO << "Opening input file: " << filepath;
	if (avformat_open_input(&m_AVFormatContext, filepath.c_str(), NULL, NULL) != 0)
	{
		std::string msg = "Failed to open file: " + filepath;
		throw std::exception(msg.c_str());
	}
	///	ptint debug log about the video file
	LOG_DEBUG << "File    : " << filepath;
	LOG_DEBUG << "Format  : " << m_AVFormatContext->iformat->name;
	LOG_DEBUG << "Duration: " << m_AVFormatContext->duration;
	LOG_DEBUG << "BitRate : " << m_AVFormatContext->bit_rate;
	LOG_DEBUG << "Finding stream info from format...";

	if (avformat_find_stream_info(m_AVFormatContext, NULL) < 0)
	{
		std::string msg = "Failed to find stream info. File: " + filepath;
		avformat_free_context(m_AVFormatContext);
		throw std::exception(msg.c_str());
	}
	/// Find the streams in the input file
	int videoStreamIndex = -1;
	int audioStreamIndex = -1;
	/// Gewt the number of streams of the input file
	m_StreamCount = m_AVFormatContext->nb_streams;
	for (int i = 0; i < m_StreamCount; i++)
	{
		AVCodecParameters* localCodecParams = nullptr;
		localCodecParams = m_AVFormatContext->streams[i]->codecpar;
		LOG_DEBUG << "AudioVideo stream time-base (fps):" << m_AVFormatContext->streams[i]->time_base.num << "/" << m_AVFormatContext->streams[i]->time_base.den;
		LOG_DEBUG << "AudioVideo stream frame-rate     :" << m_AVFormatContext->streams[i]->r_frame_rate.num << "/" << m_AVFormatContext->streams[i]->r_frame_rate.den;
		LOG_DEBUG << "AudioVideo stream start-time     :" << m_AVFormatContext->streams[i]->start_time;
		LOG_DEBUG << "AudioVideo stream duration       :" << m_AVFormatContext->streams[i]->duration;
		
		/// Get the appropriate codecs for every stream
		LOG_INFO << "Finding proper decoder...";
		AVCodec* localCodec = nullptr;
		localCodec = avcodec_find_decoder(localCodecParams->codec_id);
		if (localCodec == nullptr)
		{
			std::string msg = "Failed to find appropriate codec for file: " + filepath;
			avformat_free_context(m_AVFormatContext);
			throw std::exception(msg.c_str());
		}
		/// the stream is a video stream
		if (localCodecParams->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			if (videoStreamIndex == -1)
			{
				m_VideoStreamIndex = i;
				m_VCodec = localCodec;
				m_VCodecParameters = localCodecParams;
			}
			LOG_INFO << "Video codec:" << localCodec->long_name;
			LOG_INFO << "Resolution: " << localCodecParams->width << " x " << localCodecParams->height;
		}
		/// The stream is an audio stream
		else if (localCodecParams->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			if (audioStreamIndex == -1)
			{
				m_AudioStreamIndex = i;
				m_ACodec = localCodec;
				m_ACodecParameters = localCodecParams;
			}
			LOG_INFO << "Audio Codec:" << localCodec->long_name;
			LOG_INFO << "Channels   : " << localCodecParams->channels;
			LOG_INFO << "Sample Rate: " << localCodecParams->sample_rate;
		}
	}
	/// Allocate the context for the video codec of the input file
	m_VCodecContext = avcodec_alloc_context3(m_VCodec);
	if (!m_VCodecContext)
	{
		std::string msg = "Failed to allocate video codec context. File: " + filepath;
		avformat_free_context(m_AVFormatContext);
		throw std::exception(msg.c_str());
	}
	///	Allocate the context for the audio stream of the input file
	m_ACodecContext = avcodec_alloc_context3(m_ACodec);
	if (!m_ACodecContext)
	{
		std::string msg = "Failed to allocate audio codec context. File: " + filepath;
		avformat_free_context(m_AVFormatContext);
		throw std::exception(msg.c_str());
	}
	/// fill the video codec context with the codec parameters
	if (avcodec_parameters_to_context(m_VCodecContext, m_VCodecParameters) < 0)
	{
		std::string msg = "Failed to associate video codec parameters with video codec context. File: " + filepath;
		avcodec_free_context(&m_VCodecContext);
		avformat_free_context(m_AVFormatContext);
		throw std::exception(msg.c_str());
	}
	/// fill the audio codec context with the codec parameters
	if (avcodec_parameters_to_context(m_ACodecContext, m_ACodecParameters) < 0)
	{
		std::string msg = "Failed to associate audio codec parameters with audio codec context. File: " + filepath;
		avcodec_free_context(&m_ACodecContext);
		avformat_free_context(m_AVFormatContext);
		throw std::exception(msg.c_str());
	}
	/// Now we are ready for decoding
}
///	\brief initialize the decoder's context
///	opens the codecs for the audio and the video streams
void Transcoder::InitializeDecoderContext()
{
	if (avcodec_open2(m_VCodecContext, m_VCodec, NULL) < 0)
	{
		std::string msg = "Failed to open video codec.";
		avcodec_free_context(&m_VCodecContext);
		avformat_free_context(m_AVFormatContext);
		throw std::exception(msg.c_str());
	}
	if (avcodec_open2(m_ACodecContext, m_ACodec, NULL) < 0)
	{
		std::string msg = "Failed to open audio codec.";
		avcodec_free_context(&m_ACodecContext);
		avformat_free_context(m_AVFormatContext);
		throw std::exception(msg.c_str());
	}
	/// Temp data for decoding
	m_TempVideoFrame = av_frame_alloc();
	if (!m_TempVideoFrame)
	{
		std::string msg = "Failed to allocate frame for decoding.";
		av_frame_free(&m_TempVideoFrame);
		avcodec_free_context(&m_VCodecContext);
		avformat_free_context(m_AVFormatContext);
		throw std::exception(msg.c_str());
	}
	///
	m_TempVideoPacket = av_packet_alloc();
	if (!m_TempVideoPacket)
	{
		std::string msg = "Failed to allocate packet for decoding.";
		av_packet_free(&m_TempAudioPacket);
		avcodec_free_context(&m_VCodecContext);
		avformat_free_context(m_AVFormatContext);
		throw std::exception(msg.c_str());
	}
	/// Create temporary Buffers for Video and audio frames
	fu::Dims dims(m_VCodecContext->width, m_VCodecContext->height, 1);
	m_CurrentVideoFrame 
		= fu::Buffer<float3, fu::BufferStorageProc::CPU>::Create(dims);
	// TODO: initialize audio frame buffer
#if defined(_DEBUG) || defined (NDEBUG)
	m_DebugVideoFrame
		= fu::Buffer<unsigned char, fu::BufferStorageProc::CPU>::Create(dims);
#endif // DEBUG

}
///
void Transcoder::InitializeSwScaleContext()
{
	/// the ses context will convert the yuv values to rgbfloat32
	m_SwsContext = sws_getContext(
		m_VCodecContext->width,
		m_VCodecContext->height,
		m_VCodecContext->pix_fmt,
		m_DimScale.x,
		m_DimScale.y,
		AV_PIX_FMT_0RGB,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL
	);
}
///	\brief decode one frame of data
///	makes one decoder step
void Transcoder::DecoderStep()
{
	/// initialize the temporary video packet
	av_init_packet(m_TempVideoPacket);
	for (int i = 0; i < m_StreamCount; i++)
	{
		/// first read a packet from the video stream
		if (i == m_VideoStreamIndex)
		{
			/// read the packet from the format context
			while (av_read_frame(m_AVFormatContext, m_TempVideoPacket) >= 0)
			{
				/// send the packet to the decoder context
				DecodePacket(m_TempVideoPacket, m_VCodecContext, m_TempVideoFrame);
			}
		}
		///	read a packet from the audio stream
		else if (i == m_AudioStreamIndex)
		{
			///	send the packet to the audio decoder
			while (av_read_frame(m_AVFormatContext, m_TempAudioPacket) >= 0)
			{
				int resp = DecodePacket(m_TempAudioPacket, m_ACodecContext, m_TempAudioFrame);
			}
		}
	}
}
///
int Transcoder::DecodePacket(AVPacket* packet, AVCodecContext* codecCtx, AVFrame* frame)
{
	/// send the packet to the decoder
	int response = avcodec_send_packet(codecCtx, packet);
	if (response < 0)
	{
		char buf[64];
		av_strerror(response, buf, sizeof(buf));
		std::string msg = "Failed to send packet to codec context. Error: " + std::string(buf);
	}
	/// we've sent the packet to the decoder successfully
	if (response >= 0)
	{
		/// get the decoded frame from the decoder
		response = avcodec_receive_frame(codecCtx, frame);
		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
		{
			/// E-AGAIN means we have to send another packet
			return 0;
		}
		else if (response < 0)
		{
			std::string msg = "Failed to decode packet.";
			throw std::exception(msg.c_str());
			return response;	///
		}
		/// yeay we decoded a frame
		if (response >= 0)
		{			
			LOG_DEBUG << "Frame    : " << codecCtx->frame_number;
			LOG_DEBUG << "Type     : " << av_get_picture_type_char(frame->pict_type);
			LOG_DEBUG << "Format   : " << PixelFormat2Str.at(frame->format);
			LOG_DEBUG << "Size     : " << frame->pkt_size;
			LOG_DEBUG << "Pts      : " << frame->pts;
			LOG_DEBUG << "Key Frame: " << frame->key_frame;
			LOG_DEBUG << "Pic Num. : " << frame->coded_picture_number;
			/// copy the frame's data to an rgb buffer
			CopyFrameData2VBuffer(frame);
		}
	}
	return response;
}

void Transcoder::SetVideoScale(uint2 scale)
{
	m_DimScale = scale;
	InitializeSwScaleContext();
}

/// get current video frame buffer
const Transcoder::vframe_buffer_t& Transcoder::GetCurrentVideoFrame() const
{
	fu::Dims dims( 1, 0, 0 );
	vframe_buffer_t frame_buffer = fu::Buffer<float3, fu::BufferStorageProc::CPU>::Create(dims);
	return frame_buffer;
}
/// get curren audio frame buffer
const Transcoder::aframe_buffer_t& Transcoder::GetCurrentAudioFrame() const
{
	fu::Dims dims(1, 0, 0 );
	aframe_buffer_t frame_buffer = fu::Buffer<float, fu::BufferStorageProc::CPU>::Create(dims);
	return frame_buffer;
}

void Transcoder::Destroy()
{
	if (m_AVFormatContext)
	{
		avformat_close_input(&m_AVFormatContext);
		avformat_free_context(m_AVFormatContext);
	}
	if (&m_TempVideoPacket)
	{
		av_packet_free(&m_TempVideoPacket);
	}
	if (m_TempAudioPacket)
	{
		av_packet_free(&m_TempAudioPacket);
	}
	if (m_TempVideoFrame)
	{
		av_frame_free(&m_TempVideoFrame);
	}
	if (m_TempAudioFrame)
	{
		av_frame_free(&m_TempAudioFrame);
	}
	if (m_VCodecContext)
	{
		avcodec_free_context(&m_VCodecContext);
	}
	if (m_ACodecContext)
	{
		avcodec_free_context(&m_ACodecContext);
	}
	if (m_CurrentVideoFrame.Data())
	{
		m_CurrentVideoFrame.Deallocate();
	}
	if (m_CurrentAudioFrame.Data())
	{
		m_CurrentAudioFrame.Deallocate();
	}
}
///	\brief copy a video frame's data to current video buffer
///	\param	frame	the frame to copy
///	saves the frames data to CurrentVideFrameBuffer
void Transcoder::CopyFrameData2VBuffer(AVFrame* frame)
{
	int width = frame->width;
	int height = frame->height;
	int linesize = frame->linesize[0];
	uint8_t* data = frame->data[0];

	for (int w = 0; w < width; w++)
	{
		for (int h = 0; h < height; h++)
		{
			/// y component - luminance only
			m_DebugVideoFrame.Data()[h * width + w] = frame->data[0][h * frame->linesize[0] + w];
		}
	}
	
}
}	///	!namespace video