#ifndef __FUSION_PUBLIC_CORE_OMNICONNECTVIDEOELEMENT_H__
#define __FUSION_PUBLIC_CORE_OMNICONNECTVIDEOELEMENT_H__

#include <string>

namespace fu {
namespace fusion {

struct OmniconnectVideoElement
{
	std::string Uri;
	std::string Name;
	std::string Mentored;
	std::string DateAdded;
	std::string UploadUser;
	std::string UploadUri;
	bool		Selected{ false };
};	///	!struct OmniconnectVideoElement

using omni_video_list_ptr_t = std::shared_ptr<std::vector<OmniconnectVideoElement>>;
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_CORE_OMNICONNECTVIDEOELEMENT_H__