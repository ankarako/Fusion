#ifndef __FUSION_PUBLIC_VIEWS_OMNICONNECTVIDEOLISTVIEW_H__
#define __FUSION_PUBLIC_VIEWS_OMNICONNECTVIDEOLISTVIEW_H__

#include <Widget.h>
#include <Initializable.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>
#include <Core/OmniconnectVideoElement.h>
#include <vector>

namespace fu {
namespace fusion {

class OmniconnectVideoListView : public app::Widget, public app::Initializable
{
public:
	OmniconnectVideoListView();
	void Init() override;
	void Render() override;

	rxcpp::observer<omni_video_list_ptr_t> 	VideoListFlowIn();
	rxcpp::observable<int>					DownloadSelectedVideoIndexFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class OmniconnectVideoListView
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_VIEWS_OMNICONNECTVIDEOLISTVIEW_H__