#include <Perfcap/JpegCompressor.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <turbojpeg.h>

namespace fu {
namespace io {
namespace utils {

struct JPEGCompressorImpl
{
	Image m_OutImage;

	unsigned char* m_JpegBuffer;
	unsigned long  m_JpegBufferSize;
	unsigned int   m_CompressionQuality;
	tjhandle m_hJPEGCompressor;
	tjhandle m_hJPEGDecompressor;

	JPEGCompressorImpl() : m_JpegBuffer(NULL), m_hJPEGCompressor(NULL), m_hJPEGDecompressor(NULL), m_CompressionQuality(50) { }
};

JPEGCompressor::JPEGCompressor() : m_pImpl(NULL)
{
	m_pImpl = new JPEGCompressorImpl();
	m_pImpl->m_hJPEGCompressor = tjInitCompress();
	if (m_pImpl->m_hJPEGCompressor == NULL)
		std::cerr << "Initializing JPEG compressor failed: " << std::string(tjGetErrorStr()) << std::endl;
	m_pImpl->m_hJPEGDecompressor = tjInitDecompress();
	if (m_pImpl->m_hJPEGDecompressor == NULL)
		std::cerr << "Initializing JPEG decompressor failed: " << std::string(tjGetErrorStr()) << std::endl;
}

JPEGCompressor::~JPEGCompressor()
{
	if (m_pImpl) {

		if (m_pImpl->m_JpegBuffer != NULL)
			tjFree(m_pImpl->m_JpegBuffer);
		if (m_pImpl->m_hJPEGCompressor != NULL)
			tjDestroy(m_pImpl->m_hJPEGCompressor);
		if (m_pImpl->m_hJPEGDecompressor != NULL)
			tjDestroy(m_pImpl->m_hJPEGDecompressor);
		delete m_pImpl;
	}
}

BufferRef<unsigned char> JPEGCompressor::compressImage(const Image& image)
{
	return compressImage(image.data.getBuffer(), image.width, image.height);
}

BufferRef<unsigned char> JPEGCompressor::compressImage(const unsigned char* Image, int resX, int resY)
{
	int res = tjCompress2(m_pImpl->m_hJPEGCompressor, const_cast<unsigned char*>(Image), resX, 0, resY, TJPF_RGB, &m_pImpl->m_JpegBuffer, &m_pImpl->m_JpegBufferSize, TJSAMP_420, m_pImpl->m_CompressionQuality, 0);
	if (res < 0) {
		std::cerr << "JPEG Compression error: " << std::string(tjGetErrorStr()) << std::endl;
	}
	BufferRef<unsigned char> bref;
	bref.set(m_pImpl->m_JpegBuffer, m_pImpl->m_JpegBufferSize);
	return bref;
}

const Image& JPEGCompressor::decompressImage(const BufferRef<unsigned char>& jpegBuffer)
{
	return decompressImage(jpegBuffer.getBuffer(), jpegBuffer.getSizeInBytes());
}

const Image& JPEGCompressor::decompressImage(const unsigned char* jpegBuffer, unsigned long bufferSizeInBytes)
{
	int w, h, jpegSubSamp;
	int res = tjDecompressHeader2(m_pImpl->m_hJPEGDecompressor, const_cast<unsigned char*>(jpegBuffer), bufferSizeInBytes, &w, &h, &jpegSubSamp);
	if (res < 0)
		std::cerr << "Error reading JPEG header: " << std::string(tjGetErrorStr()) << std::endl;
	m_pImpl->m_OutImage.width = w;
	m_pImpl->m_OutImage.height = h;
	m_pImpl->m_OutImage.data.resize(4 * w*h);
	res = tjDecompress2(m_pImpl->m_hJPEGDecompressor, const_cast<unsigned char*>(jpegBuffer), bufferSizeInBytes, m_pImpl->m_OutImage.data.getBuffer(), w, w*tjPixelSize[TJPF_RGBX], h, TJPF_RGBX, 0);
	if (res < 0)
		std::cerr << "Error decompressing JPEG image: " << std::string(tjGetErrorStr()) << std::endl;
	return m_pImpl->m_OutImage;
}


unsigned int JPEGCompressor::getCompressionQuality() const
{
	return m_pImpl->m_CompressionQuality;
}

void JPEGCompressor::setCompressionQuality(unsigned int q)
{
	m_pImpl->m_CompressionQuality = std::max<unsigned int>(std::min<unsigned int>(q, 100), 1);
}
}	/// !namespace utils
}	///	!namespace io
}	///	!namespace fu