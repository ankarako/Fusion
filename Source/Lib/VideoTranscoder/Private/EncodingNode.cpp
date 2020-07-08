#include <EncodingNode.h>

namespace fu {
namespace trans {
///	\struct Impl
///	\brief EncodingNodeObj Implementation
struct EncodingNodeObj::Impl
{
	static constexpr const char* k_FfmpegPath = "Resources\\ffmpeg\\ffmpeg.exe";
	std::string m_OutputFilepath;
	std::string m_InputFilenamePrefix;
	std::string m_InputDirectory;
	Impl() { }
};	///	!struct Impl
/// Construction
EncodingNodeObj::EncodingNodeObj()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
/// 
void EncodingNodeObj::SetInputDirectory(const std::string& inputDir)
{
	m_Impl->m_InputDirectory = inputDir;
}
void EncodingNodeObj::SetInputFilenamePrefix(const std::string& prefix)
{
	m_Impl->m_InputFilenamePrefix = prefix;
}
void EncodingNodeObj::SetOutputFilepath(const std::string& filepath)
{
	m_Impl->m_OutputFilepath = filepath;
}
void EncodingNodeObj::ExportVideo()
{
	std::string cli = m_Impl->k_FfmpegPath + std::string(" -start_number 0 -r 29.99 -i ") + m_Impl->m_InputDirectory + "\\texels_%04d.png -c:v mjpeg -qscale:v 0 " + m_Impl->m_OutputFilepath;
	std::system(cli.c_str());
}
}	///	!namespace trans
}	///	!namespace fu