#include <WidgetRepo.h>
#include <Widget.h>

#include <algorithm>

namespace app {
///	\struct Impl
///	\brief WidgetRepo implementation
struct WidgetRepo::Impl
{
	std::vector<widget_ptr_t> m_Widgets;

	Impl(const std::vector<widget_ptr_t>& widgets)
		: m_Widgets(widgets)
	{ }
};	///	!struct Impl
///	Construction
WidgetRepo::WidgetRepo(const std::vector<widget_ptr_t>& widgets)
	: m_Impl(spimpl::make_unique_impl<Impl>(widgets))
{ }
/// \brief register a widget
///	\param	widget	the widget to register
void WidgetRepo::RegisterWidget(widget_ptr_t w)
{
	if (std::find(m_Impl->m_Widgets.begin(), m_Impl->m_Widgets.end(), w) == m_Impl->m_Widgets.end())
	{
		m_Impl->m_Widgets.push_back(w);
	}
}

void WidgetRepo::UnregisterWidget(widget_ptr_t w)
{
	m_Impl->m_Widgets.erase(
		std::remove_if(
			m_Impl->m_Widgets.begin(), m_Impl->m_Widgets.end(), 
				[w](auto x)
			{ 
				return x == w; 
			}), m_Impl->m_Widgets.end());
}

void WidgetRepo::Render()
{
	std::vector<widget_ptr_t> ws = m_Impl->m_Widgets;
	for (auto w : ws)
		w->Render();
}
}	///	!namespace app