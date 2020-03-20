#ifndef	__IO_PUBLIC_PERFCAP_TEXTUREFRAME_H__
#define __IO_PUBLIC_PERFCAP_TEXTUREFRAME_H__

#include <Perfcap/TextureAttribute.h>
#include <vector>

namespace fu {
namespace io {

struct TextureFrame
{
	std::vector<TextureAttribute> 			TextureAttributes;
	std::vector<std::vector<unsigned char>>	TextureBuffers;

	void reset()
	{
		TextureAttributes.clear();
		TextureBuffers.clear();
	}

	bool operator==(const TextureFrame& other) const
	{
		return TextureAttributes == other.TextureAttributes
			&& TextureBuffers == other.TextureBuffers;
	}

	template <typename F>
	void serialize(F& buf) const
	{
		buf << TextureAttributes << TextureBuffers;
	}

	template <typename F>
	void parse(F& buf)
	{
		buf >> TextureAttributes >> TextureBuffers;
	}
};	///	!TextureFrame
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAP_TEXTUREFRAME_H__