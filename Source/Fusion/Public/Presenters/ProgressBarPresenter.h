#ifndef __FUSION_PUBLIC_PRESENTERS_PROGRESSBARPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_PROGRESSBARPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace app {
class WidgetRepo;
}
namespace fusion {

class ProgressBarView;

class ProgressBarPresenter : public app::Initializable
{
public:
	using prog_view_ptr_t = std::shared_ptr<ProgressBarView>;
	using wrepo_ptr_t = std::shared_ptr<app::WidgetRepo>;

	ProgressBarPresenter(prog_view_ptr_t prog_view, wrepo_ptr_t wrepo);
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_PROGRESSBARPRESENTER_H__