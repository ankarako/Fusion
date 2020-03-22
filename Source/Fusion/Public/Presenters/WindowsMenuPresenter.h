#ifndef	__FUSION_PUBLIC_PRESENTERS_WINDOWSMENUPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_WINDOWSMENUPRESENTER_H__

#include <spimpl.h>

namespace fu {
namespace fusion {

class WindowsMenuPresenter
{
public:
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FileMenuPresenter
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_WINDOWSMENUPRESENTER_H__