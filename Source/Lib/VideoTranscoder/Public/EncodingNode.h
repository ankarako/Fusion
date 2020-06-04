#ifndef __VIDEOTRANSCODER_PUBLIC_ENCODINGNODE_H__
#define __VIDEOTRANSCODER_PUBLIC_ENCODINGNODE_H__

#include <spimpl.h>
#include <string>

namespace fu {
namespace trans {

class EncodingNodeObj
{
public:
	EncodingNodeObj();
	void SetInputDirectory(const std::string& inputDir);
	void SetInputFilenamePrefix(const std::string& prefix);
	void SetOutputFilepath(const std::string& filepath);
	void ExportVideo();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class EncodingNodeObj
///	\typedef EncodingNode
///	\brief	a shared EncodingNodeObj object
using EncodingNode = std::shared_ptr<EncodingNodeObj>;
///	\brief	create an EncodingNode object
static EncodingNode CreateEncodingNode()
{
	return std::make_shared<EncodingNodeObj>();
}
}	///	!namespace trans
}	///	!namespace fu
#endif	///	!__VIDEOTRANSCODER_PUBLIC_ENCODINGNODE_H__