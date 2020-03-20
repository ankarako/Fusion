#ifndef __IO_PUBLIC_PERFCAP_JPEGCOMPRESSOR_H__
#define __IO_PUBLIC_PERFCAP_JPEGCOMPRESSOR_H__

#include <Perfcap/BufferRef.h>

namespace fu {
namespace io {
namespace utils {
struct Image
{
	int width;
	int height;
	Buffer<unsigned char> data;
};

struct JPEGCompressorImpl;

class JPEGCompressor
{
public:

	JPEGCompressor();
	~JPEGCompressor();

	BufferRef<unsigned char> compressImage(const Image& image);
	BufferRef<unsigned char> compressImage(const unsigned char* Image, int resX, int resY);

	const Image& decompressImage(const BufferRef<unsigned char>& jpegBuffer);
	const Image& decompressImage(const unsigned char* jpegBuffer, unsigned long bufferSizeInBytes);

	unsigned int getCompressionQuality() const;
	void setCompressionQuality(unsigned int q);

private:

	JPEGCompressorImpl * m_pImpl;
};
}	///	!namespace utils
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAP_JPEGCOMPRESSOR_H__