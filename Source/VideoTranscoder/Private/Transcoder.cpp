#include <Transcoder.h>
#include <plog/Log.h>

namespace video {
///	Construction
///	\brief default constructor
Transcoder::Transcoder() { }
/// Destruction
Transcoder::~Transcoder()
{
	Destroy();
}
///	\brief Initialize the transcoding context
///	Allocates an av format context
void Transcoder::InitializeContext()
{ 
	m_AVFormatContext = avformat_alloc_context();
	if (!m_AVFormatContext)
		throw std::exception("Failed to allocate AV Context.");
}
///
void Transcoder::LoadFile(const std::string& filepath)
{
	LOG_INFO << "Opening input file: " << filepath;
	if (avformat_open_input(&m_AVFormatContext, filepath.c_str(), NULL, NULL) != 0)
	{
		std::string msg = "Failed to open file: " + filepath;
		throw std::exception(msg.c_str());
	}
	LOG_INFO << "File    : " << filepath;
	LOG_INFO << "Format  : " << m_AVFormatContext->iformat->name;
	LOG_INFO << "Duration: " << m_AVFormatContext->duration;
	LOG_INFO << "BitRate : " << m_AVFormatContext->bit_rate;
	LOG_INFO << "Finding stream info from format...";
	if (avformat_find_stream_info(m_AVFormatContext, NULL) < 0)
	{
		std::string msg = "Failed to find stream info. File: " + filepath;
		throw std::exception(msg.c_str());
	}
	int videoStreamIndex = -1;
	int audioStreamIndex = -1;
	m_StreamCount = m_AVFormatContext->nb_streams;
	for (int i = 0; i < m_StreamCount; i++)
	{
		AVCodecParameters* localCodecParams = nullptr;
		localCodecParams = m_AVFormatContext->streams[i]->codecpar;
		LOG_INFO << "AudioVideo stream time-base (fps):" << m_AVFormatContext->streams[i]->time_base.num << "/" << m_AVFormatContext->streams[i]->time_base.den;
		LOG_INFO << "AudioVideo stream frame-rate     :" << m_AVFormatContext->streams[i]->r_frame_rate.num << "/" << m_AVFormatContext->streams[i]->r_frame_rate.den;
		LOG_INFO << "AudioVideo stream start-time     :" << m_AVFormatContext->streams[i]->start_time;
		LOG_INFO << "AudioVideo stream duration       :" << m_AVFormatContext->streams[i]->duration;
		
		LOG_INFO << "Finding proper decoder...";
		AVCodec* localCodec = nullptr;
		localCodec = avcodec_find_decoder(localCodecParams->codec_id);

		if (localCodec == nullptr)
		{
			std::string msg = "Failed to find appropriate codec for file: " + filepath;
			throw std::exception(msg.c_str());
		}

		if (localCodecParams->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			if (videoStreamIndex == -1)
			{
				m_VideoStreamIndex = i;
				m_VCodec = localCodec;
				m_VCodecParameters = localCodecParams;
			}
			LOG_INFO << "Video codec:" << localCodec->name;
			LOG_INFO << "Resolution: " << localCodecParams->width << " x " << localCodecParams->height;
		}
		else if (localCodecParams->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			if (audioStreamIndex == -1)
			{
				m_AudioStreamIndex = i;
				m_ACodec = localCodec;
				m_ACodecParameters = localCodecParams;
			}
			LOG_INFO << "Audio Codec:" << localCodec->name;
			LOG_INFO << "Channels   : " << localCodecParams->channels;
			LOG_INFO << "Sample Rate: " << localCodecParams->sample_rate;
		}
	}
	m_VCodecContext = avcodec_alloc_context3(m_VCodec);
	if (!m_VCodecContext)
	{
		std::string msg = "Failed to allocate video codec context. File: " + filepath;
		throw std::exception(msg.c_str());
	}
	m_ACodecContext = avcodec_alloc_context3(m_ACodec);
	if (!m_ACodecContext)
	{
		std::string msg = "Failed to allocate audio codec context. File: " + filepath;
		throw std::exception(msg.c_str());
	}
	/// fill the codec context with the codec parameters
	if (avcodec_parameters_to_context(m_VCodecContext, m_VCodecParameters) < 0)
	{
		std::string msg = "Failed to associate video codec parameters with video codec context. File: " + filepath;
		throw std::exception(msg.c_str());
	}
	if (avcodec_parameters_to_context(m_ACodecContext, m_ACodecParameters) < 0)
	{
		std::string msg = "Failed to associate audio codec parameters with audio codec context. File: " + filepath;
		throw std::exception(msg.c_str());
	}
}
///
void Transcoder::InitializeDecoderContext()
{
	if (avcodec_open2(m_VCodecContext, m_VCodec, NULL) < 0)
	{
		std::string msg = "Failed to open video codec.";
		throw std::exception(msg.c_str());
	}
	if (avcodec_open2(m_ACodecContext, m_ACodec, NULL) < 0)
	{
		std::string msg = "Failed to open audio codec.";
		throw std::exception(msg.c_str());
	}
	///
	m_TempVideoFrame = av_frame_alloc();
	if (!m_TempVideoFrame)
	{
		std::string msg = "Failed to allocate frame for decoding.";
		throw std::exception(msg.c_str());
	}
	///
	m_TempVideoPacket = av_packet_alloc();
	if (!m_TempVideoPacket)
	{
		std::string msg = "Failed to allocate packet for decoding.";
		throw std::exception(msg.c_str());
	}
	/// Create Buffers for Video and audio frames
	fu::Dims dims(m_VCodecContext->width, m_VCodecContext->height, 1);
	m_CurrentVideoFrame 
		= fu::Buffer<float, fu::BufferStorageProc::CPU>::Create(dims);
	/*m_CurrentAudioFrame
		= fu::Buffer<float, fu::BufferStorageProc::CPU>::Create(fu::Dims{ m_VCodecContext->width, m_VCodecContext->height, 1 });*/

}
///
void Transcoder::InitializeSwScaleContext()
{
	m_SwsContext = sws_getContext(
		m_VCodecContext->width,
		m_VCodecContext->height,
		m_VCodecContext->pix_fmt,
		m_VCodecContext->width,
		m_VCodecContext->height,
		AV_PIX_FMT_0RGB32,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL
	);
}
///
void Transcoder::DecoderStep()
{
	av_init_packet(m_TempVideoPacket);
	for (int i = 0; i < m_StreamCount; i++)
	{
		if (i == m_VideoStreamIndex)
		{
			while (av_read_frame(m_AVFormatContext, m_TempVideoPacket) >= 0)
			{
				DecodePacket(m_TempVideoPacket, m_VCodecContext, m_TempVideoFrame);
				//if (frameFinished)
				//{
				//	AVPicture pic;
				//}
				
			}
		}
		else if (i == m_AudioStreamIndex)
		{
			if (av_read_frame(m_AVFormatContext, m_TempAudioPacket) >= 0)
			{
				int resp = DecodePacket(m_TempAudioPacket, m_ACodecContext, m_TempAudioFrame);
			}
		}
	}
}
///
int Transcoder::DecodePacket(AVPacket* packet, AVCodecContext* codecCtx, AVFrame* frame)
{
	/// supply raw data as input to the decoder
	int response = avcodec_send_packet(codecCtx, packet);
	if (response < 0)
	{
		char buf[64];
		av_strerror(response, buf, sizeof(buf));
		std::string msg = "Failed to send packet to codec context. Error: " + std::string(buf);
		//LOG_ERROR << msg;
	}
	while (response >= 0)
	{
		/// return the decoded output data into the frame
		response = avcodec_receive_frame(codecCtx, frame);
		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
		{
			char buf[64];
			av_strerror(response, buf, sizeof(buf));
			std::string msg = "Failed to send packet to codec context. Error: " + std::string(buf);
			//LOG_WARNING << msg;
			break;
		}
		else if (response < 0)
		{
			std::string msg = "Failed to receive frame from the decoder.";
			throw std::exception(msg.c_str());
			return response;	///
		}

		if (response >= 0)
		{
			LOG_DEBUG << "Frame    : " << codecCtx->frame_number;
			LOG_DEBUG << "Type     : " << av_get_picture_type_char(frame->pict_type);
			LOG_DEBUG << "Format   : " << frame->format;
			LOG_DEBUG << "Size     : " << frame->pkt_size;
			LOG_DEBUG << "Pts      : " << frame->pts;
			LOG_DEBUG << "Key Frame: " << frame->key_frame;
			LOG_DEBUG << "Pic Num. : " << frame->coded_picture_number;
			LOG_WARNING << frame->data;
			LOG_WARNING << frame->linesize;
			memcpy(m_CurrentVideoFrame.Data(), frame->data, m_CurrentVideoFrame.GetByteSize());
		}
	}
	return 0;
}
/// get current video frame buffer
const Transcoder::vframe_buffer_t& Transcoder::GetCurrentVideoFrame() const
{
	fu::Dims dims( 1, 0, 0 );
	vframe_buffer_t frame_buffer = fu::Buffer<float, fu::BufferStorageProc::CPU>::Create(dims);
	return frame_buffer;
}
/// get curren audio frame buffer
const Transcoder::aframe_buffer_t& Transcoder::GetCurrentAudioFrame() const
{
	fu::Dims dims(1, 0, 0 );
	vframe_buffer_t frame_buffer = fu::Buffer<float, fu::BufferStorageProc::CPU>::Create(dims);
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
}
}