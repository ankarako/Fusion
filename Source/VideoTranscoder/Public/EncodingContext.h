#ifndef __VIDEOTRANSCODER_PUBLIC_ENCODINGCONTEXT_H__
#define __VIDEOTRANSCODER_PUBLIC_ENCODINGCONTEXT_H__

namespace fu {
namespace trans {
class EncodingContext
{
public:
	///	Construction
	///	\brief default constructor
	EncodingContext();
	///	\brief initialize the context
	bool Initialize();
	///	\brief Terminate the context
	void Terminate();
private:
	void* data{ nullptr };
};	///	!class EncodingContext
}	///	!namespace trans
}	///	!namespace fu
#endif	///	!__VIDEOTRANSCODER_PUBLIC_ENCODINGCONTEXT_H__