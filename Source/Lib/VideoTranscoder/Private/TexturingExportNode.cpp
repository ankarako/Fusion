#include <TextureExportingNode.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

namespace fu {
namespace trans {
struct TextureExportingNodeObj::Impl
{
	Impl() { }
};	///	!
TextureExportingNodeObj::TextureExportingNodeObj()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
///
void TextureExportingNodeObj::ExportTexture(const std::string& filepath, const BufferCPU<uchar4>& buf, const uint2& size)
{
	cv::Mat mat = cv::Mat::zeros(cv::Size(size.x, size.y), CV_8UC4);
	std::memset(mat.data, 204, 4 * size.x * size.y * sizeof(unsigned char));
	std::memcpy(mat.data, buf->Data(), buf->ByteSize());
	cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGR);
	cv::imwrite(filepath, mat);
}

void TextureExportingNodeObj::ExportWeights(const std::string& filepath, const BufferCPU<uchar4>& buf, const uint2& size)
{
	cv::Mat mat = cv::Mat::zeros(cv::Size(size.x, size.y), CV_8UC4);
	std::memcpy(mat.data, buf->Data(), buf->ByteSize());
	cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGR);
	cv::imwrite(filepath, mat);
}
}	///	!namespace trans
}	///	!namespace fu