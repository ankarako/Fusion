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
///	\brief load a meadia file
///	\param	filepath	the incoming file's path
///	\return true if the file was loaded successfully, false otherwise
bool LoadSource(DecodingContext context, const std::wstring& filepath);
}	///	!namespace dec
/////	\struct TranscodingContext
/////	\brief transcoding context state (opaque to the user)
//struct TranscodingContextObj;
//
/////	\typedef TranscodingContext
/////	\brief a reference counted TranscodingContext
//using TranscodingContext = std::shared_ptr<TranscodingContextObj>;
/////	\brief create a TranscodingContext
/////	\param	version	the version of the context to create
/////	\return true if the context was created successfully, false otherwise
//DecodingContext CreateDecodingContext();
/////	\brief shutdown a decoding context
/////	Releases the Windows Media Foundation context
//void DestroyDecodingContext(DecodingContext context);
}	///	!namespace trans
}	///	!namespace fu
#endif	///	!__VIDEOTRANSCODER_PUBLIC_TRANSCODER_H__
