#include <Presenters/ProgressBarPresenter.h>
#include <Views/ProgressBarView.h>
#include <WidgetRepo.h>

namespace fu {
namespace fusion {
struct ProgressBarPresenter::Impl
{
	prog_view_ptr_t m_View;
	wrepo_ptr_t		m_Wrepo;

	Impl(prog_view_ptr_t view, wrepo_ptr_t wrepo)
		: m_View(view)
		, m_Wrepo(wrepo)
	{ }
};

ProgressBarPresenter::ProgressBarPresenter(prog_view_ptr_t view, wrepo_ptr_t wrepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, wrepo))
{ }

void ProgressBarPresenter::Init()
{
	m_Impl->m_View->OnActivated()
		.subscribe([this](auto _) 
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_View);
	});

	m_Impl->m_View->OnDeactivated()
		.subscribe([this](auto _) 
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_View);
	});
}
}	///	!namespace fusion
}	///	!namespace fu