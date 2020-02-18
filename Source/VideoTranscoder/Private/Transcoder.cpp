#include <Transcoder.h>
#include <plog/Log.h>

namespace video {
///	Construction
///	\brief default constructor
Transcoder::Transcoder() { }
///	\brief Initialize the transcoding context
///	Allocates an av format context
void Transcoder::Initialize()
{
	m_FormatContext = avformat_alloc_context();
	if (!m_FormatContext)
		LOG_ERROR << "Failed to allocate memory for Format context.";
}
void Transcoder::LoadFile(const std::string& filepath)
{
	LOG_INFO << "Opening file: " << filepath;
	if (!avformat_open_input(&m_FormatContext, filepath.c_str(), NULL, NULL))
	{
		throw std::runtime_error("Failed to load file: " + filepath);
	}
	LOG_INFO << "File: " << filepath;
	LOG_INFO << "Format  : " << m_FormatContext->iformat->name;
	LOG_INFO << "Duration: " << m_FormatContext->duration;
	LOG_INFO << "Bit Rate: " << m_FormatContext->bit_rate;
}
}