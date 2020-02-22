#ifndef __VIDEOTRANSCODER_PUBLIC_DECODING_CONTEXT_H__
#define __VIDEOTRANSCODER_PUBLIC_DECODING_CONTEXT_H__

#include <Stream.h>
#include <Rational.h>
#include <ffmpegcpp.h>

#ifndef _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
	#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif	///	!_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <experimental/filesystem>
#include <string>
#include <memory>
#include <type_traits>
#include <plog/Log.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace trans {
///	namespace usage
namespace fs = std::experimental::filesystem;
///	\class DecodingContext
///	\brief a decoding context interface
///	\tparam	the decoded frame output type
///	\note: the output frame type specifies the type of the decoder too
///	(i.e.) if the decoder is a video decoder or an audio decoder
class DecodingContext
{
public:
	///	\typedef is_video_decoder
	///	\brief a bool constant to check if this is a video encoder
	///	\typedef stream_ptr_t
	///	\brief a pointer to a stream
	using stream_ptr_t	= std::shared_ptr<Stream>;
	///	\typedef frame_t
	///	\brief the output frame type
	// using frame_t = OutputFrameType;
	///	\typedef frame_ptr_t
	///	\brief the outptu frame poiinter type
	// using frame_ptr_t = std::shared_ptr<frame_t>;
	///	\brief initialize the context
	///	Initiliazes the AVFormatContext of the decoder and sets
	///	ffmegs log level
	///	\return true if the context was initialized successfully
	bool Initialize()
	{
		/// allocate memory for the context
		m_AVFormatContext = avformat_alloc_context();
		if (!m_AVFormatContext)
		{
			LOG_ERROR << "Failed to initialize decoding context.";
			LOG_ERROR << "\tFailed to allocate memory for AVFormatContext";
			return false;
		}
		//av_register_all();
		#if defined(_DEBUG) || defined(NDEBUG)
			av_log_set_level(AV_LOG_ERROR);
		#else
			av_log_set_level(AV_LOG_ERROR);
		#endif
		return true;
	}
	///	\brief open input media file
	///	Opens the given file and finds the streams of the container
	///	for every stream it finds an appropriate codec for decoding it
	///	to raw data
	///	\return true, if all the file was loaded successfully, false otherwise
	bool LoadFile(const std::string& filepath)
	{
		std::string filename = fs::path(filepath).filename().generic_string();
		LOG_INFO << "Opening input file: " << filename;
		if (avformat_open_input(&m_AVFormatContext, filepath.c_str(), NULL, NULL) != 0)
		{
			LOG_ERROR << "Failed to load input file: " << filename;
			Terminate();
			Initialize();
			return false;
		}
		LOG_DEBUG << "File    : " << filename;
		LOG_DEBUG << "Format  : " << m_AVFormatContext->iformat->long_name;
		LOG_DEBUG << "Duration: " << m_AVFormatContext->duration;
		LOG_DEBUG << "Bit Rate: " << m_AVFormatContext->bit_rate;
		
		LOG_INFO << "Finding stream info from file format...";
		if (avformat_find_stream_info(m_AVFormatContext, NULL) < 0)
		{
			LOG_ERROR << "Failed to find stream info for file: " << filename;
			Terminate();
			Initialize();
			return false;
		}
		/// find the streams of the input file
		int numStreams = m_AVFormatContext->nb_streams;
		for (int i = 0; i < numStreams; i++)
		{
			stream_ptr_t stream = std::make_shared<Stream>();
			stream->Id = i;
			stream->CodecParams = m_AVFormatContext->streams[i]->codecpar;
			stream->Codec = avcodec_find_decoder(stream->CodecParams->codec_id);
			if (!stream->Codec)
			{
				LOG_ERROR << "Failed to find the appropriate decoder for file: " << filename;
				LOG_ERROR << "Stream: " << stream->Id;
				LOG_ERROR << "Codec Tag: " << stream->CodecParams->codec_tag;
				Terminate();
				Initialize();
				return false;
			}
			/// find the type of the stream
			if (stream->CodecParams->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				LOG_DEBUG << "Found Video Stream: ";
				LOG_DEBUG << "Id        : " << stream->Id;
				LOG_DEBUG << "Type      : " << StreamType2Str.at(StreamType::Video);
				LOG_DEBUG << "Codec     : " << stream->Codec->long_name;
				LOG_DEBUG << "Resolution: " << stream->CodecParams->width << " x " << stream->CodecParams->height;
				m_Streams[StreamType::Video] = stream;
			}
			else if (stream->CodecParams->codec_type == AVMEDIA_TYPE_AUDIO)
			{
				LOG_DEBUG << "Found Audio Stream: ";
				LOG_DEBUG << "Id         : " << stream->Id;
				LOG_DEBUG << "Type       : " << StreamType2Str.at(StreamType::Audio);
				LOG_DEBUG << "Codec      : " << stream->Codec->long_name;
				LOG_DEBUG << "Channels   : " << stream->CodecParams->channels;
				LOG_DEBUG << "Sample Rate: " << stream->CodecParams->sample_rate;
				m_Streams[StreamType::Audio] = stream;
			}
			/// allocate codec context
			stream->CodecContext = avcodec_alloc_context3(stream->Codec);
			if (!stream->CodecContext)
			{
				LOG_ERROR << "Failed to allocate codec context for stream: " << stream->Id;
				LOG_ERROR << "Stream type: " << StreamType2Str.at(GetStreamType(stream));
				Terminate();
				Initialize();
				return false;
			}
			/// fill the codec context
			if (avcodec_parameters_to_context(stream->CodecContext, stream->CodecParams) < 0)
			{
				LOG_ERROR << "Failed to fill the stream's codec context with its codec parameters.";
				LOG_ERROR << "Stream type: " << StreamType2Str.at(GetStreamType(stream));
				Terminate();
				Initialize();
				return false;
			}
			/// Open codecs and allocate temp packets and frames for decoding
			///	Open codec for stream
			if (avcodec_open2(stream->CodecContext, stream->Codec, NULL) < 0)
			{
				LOG_ERROR << "Failed to initialize codec for decoding.";
				LOG_ERROR << "Stream Id  : " << stream->Id;
				LOG_ERROR << "Stream type: " << StreamType2Str.at(GetStreamType(stream));
				Terminate();
				Initialize();
			}
			///	allocate frame for stream
			stream->Frame = av_frame_alloc();
			if (!stream->Frame)
			{
				LOG_ERROR << "Failed to allocate frame for stream: " << stream->Id;
				LOG_ERROR << "Stream type: " << StreamType2Str.at(GetStreamType(stream));
				Terminate();
				Initialize();
				return false;
			}
			stream->Packet = av_packet_alloc();
			if (!stream->Packet)
			{
				LOG_ERROR << "Failed to allocate packet for stream: " << stream->Id;
				LOG_ERROR << "Stream type: " << StreamType2Str.at(GetStreamType(stream));
				Terminate();
				Initialize();
				return false;
			}
			av_init_packet(stream->Packet);
		}
		return m_IsFileLoaded = true;
	}
	///	\brief go to a specific frame in the file
	///	\param	frameId	the frame id of the frame to seek
	void GoToFrame(unsigned int frameId)
	{
		int seekflags = 0;
		for (auto stream : m_Streams)
		{
			if (frameId < m_CurrentFrameIndex)
			{
				seekflags = AVSEEK_FLAG_BACKWARD;
			}
			/// seek
		}
	}
	///	\brief decode one frame
	///	\return true if the current frame was decoded successfully
	bool DecodeCurrentFrame()
	{
		for (auto& stream : m_Streams)
		{
			while(av_read_frame(m_AVFormatContext, stream.second->Packet) >= 0)
			{
				int send_resp = avcodec_send_packet(stream.second->CodecContext, stream.second->Packet);
				if( send_resp < 0)
				{
					LOG_ERROR << "Failed to send packet to the decoder.";
					LOG_ERROR << "Stream Id  : " << stream.second->Id;
					LOG_ERROR << "Stream type: " << StreamType2Str.at(stream.first);
					char buf[64];
					av_strerror(send_resp, buf, sizeof(buf));
					LOG_ERROR << "[ffmpeg] Error: " << buf;
					Terminate();
					Initialize();
					return false;
				}
				///	get the response from the decodee
				int receive_resp = avcodec_receive_frame(stream.second->CodecContext, stream.second->Frame);
				if (receive_resp == AVERROR(EAGAIN) || receive_resp == AVERROR_EOF)
				{
					continue;
				}
				else if (receive_resp < 0)
				{
					LOG_ERROR << "Failed to receive frame from the decoder.";
					LOG_ERROR << "Stream Id  : " << stream.second->Id;
					LOG_ERROR << "Stream type: " << StreamType2Str.at(stream.first);
					char buf[64];
					av_strerror(AVERROR(receive_resp), buf, sizeof(buf));
					LOG_ERROR << "[ffmpeg] Error: " << buf;
					Terminate();
					Initialize();
					return false;
				}
				else
				{
					LOG_DEBUG << "Received frame from the decoder.";
					LOG_DEBUG << "Frame     : " << stream.second->CodecContext->frame_number;
					LOG_DEBUG << "Type      : " << av_get_picture_type_char(stream.second->Frame->pict_type);
					LOG_DEBUG << "Size      : " << stream.second->Frame->pkt_size;
					LOG_DEBUG << "pts       : " << stream.second->Frame->pts;
					LOG_DEBUG << "Keyframe  : " << stream.second->Frame->key_frame;
					LOG_DEBUG << "Pict. code: " << stream.second->Frame->coded_picture_number;
					return true;
				}
			}
		}
		return true;
	}
	///	\brief Terminate the context
	///	Deallocates everything
	void Terminate()
	{
		if (m_IsFileLoaded)
		{
			avformat_close_input(&m_AVFormatContext);
		}
		if (m_AVFormatContext)
		{
			avformat_free_context(m_AVFormatContext);
		}
		if (m_SwScaleContext)
		{
			sws_freeContext(m_SwScaleContext);
		}
		for (auto& stream : m_Streams)
		{
			if (stream.second->Frame)
			{
				av_frame_free(&stream.second->Frame);
			}
			if (stream.second->Packet)
			{
				av_packet_free(&stream.second->Packet);
			}
			if (stream.second->CodecContext)
			{
				avcodec_free_context(&stream.second->CodecContext);
			}
		}
	}
	///	\brief Get the current stream data of the specified stream type
	///	\param	streamType	the stream type 
	///	\return the specified type's stream-data
	stream_ptr_t GetStreamData(StreamType streamType)
	{
		if (m_Streams.find(streamType) == m_Streams.end())
		{
			LOG_ERROR << "The specified stream could not be found.";
			throw std::runtime_error("The specified stream could not be found");
		}
		return m_Streams[streamType];

	}
	///	\brief get the frame rate of the video stream of the loaded file
	///	\return the frame rate of the loaded file's vide stream
	Rational GetVideoStreamFrameRate() const
	{
		auto pos = m_Streams.find(StreamType::Video);
		if (pos != m_Streams.end())
		{
			auto stream = m_Streams.at(StreamType::Video);
			return Rational{ stream->CodecContext->framerate.num , stream->CodecContext->framerate.den };
		}
		return Rational{ 0 , 0 };;
	}
	///	\brief check if there is a loaded file in the context
	///	\return true if there is a loaded file in the context, false otherwise
	bool IsFileLoaded() const
	{
		return m_IsFileLoaded;
	}
private:
	///	\brief get the stream type of the specified stream
	///	linear search over m_Streams member
	///	\param	stream	the stream to search for its stream type
	///	\return the stream type of the specified stream
	StreamType GetStreamType(stream_ptr_t stream)
	{
		for (auto it = m_Streams.begin(); it != m_Streams.end(); it++)
		{
			if (it->second == stream)
				return it->first;
		}
	}
private:
	AVFormatContext*					m_AVFormatContext{ nullptr };
	SwsContext*							m_SwScaleContext{ nullptr };
	std::map<StreamType, stream_ptr_t> 	m_Streams;
	bool 								m_IsFileLoaded{ false };
	/// TODO: maybe these should be settings for a higher level logic
	unsigned int				m_CurrentFrameIndex{ 0u };
	std::string					m_LoadedFile{ "" };
	size_t						m_OpenedFileDuration{ 0 };	///	TODO: understand what it is
};	///	!class DecodingContext
}	///	!namespace trans
}	///	!namespace fu
#endif	///	!__VIDEOTRANSCODER_PUBLIC_DECODING_CONTEXT_H__