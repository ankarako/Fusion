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
}