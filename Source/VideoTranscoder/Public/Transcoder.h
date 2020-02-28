#ifndef __VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__
#define __VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__

#include <string>
#include <memory>

namespace fu {
namespace trans {
///	\struct DecodingContextObj
///	\brief 
struct DecodingContextObj;
///	\typedef DecodingContext
///	\brief a reference counted DecodingContextObj
using DecodingContext = std::shared_ptr<DecodingContextObj>;
namespace dec {
///	\brief create a context
///	allocates a decodding object pointer
DecodingContext CreateContext();
///	\brief destroy the decoding context
void DestroyContext(DecodingContext context);
///	\brief load a meadia file
///	\param	filepath	the incoming file's path
///	\return true if the file was loaded successfully, false otherwise
bool LoadSource(DecodingContext context, const std::wstring& filepath);
///	\brief add a video transform to the decoding context
///	\param	Transform	the transform to add
///	\return true if the transform was added successfully, false otherwise
bool AddTransform();
}	///	!namespace dec
}	///	!namespace trans
}	///	!namespace fu
#endif	///	!__VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__
