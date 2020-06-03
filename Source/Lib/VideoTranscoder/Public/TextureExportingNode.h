#ifndef __VIDEOTRANSCODER_PUBLIC_TEXTUREEXPROTINGNODE_H__
#define __VIDEOTRANSCODER_PUBLIC_TEXTUREEXPROTINGNODE_H__

#include <Buffer.h>
#include <string>
#include <spimpl.h>

namespace fu {
namespace trans {
///	\class TextureExportingNodeObj
///	\brief exports texture maps
class TextureExportingNodeObj
{
public:
	TextureExportingNodeObj();
	void ExportTexture(const std::string& filepath, const BufferCPU<uchar4>& buf, const uint2& size);
	void ExportWeights(const std::string& filepath, const BufferCPU<uchar4>& buf, const uint2& size);
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class TextureExportingNodeObj
///	\typedef 
///	\brief 
using TextureExportingNode = std::shared_ptr<TextureExportingNodeObj>;
///	\brief
static TextureExportingNode CreateTextureExportingNode()
{
	return std::make_shared<TextureExportingNodeObj>();
}
}	///	!namespace trans
}	///	!namespace fu
#endif ///	!__VIDEOTRANSCODER_PUBLIC_TEXTUREEXPROTINGNODE_H__