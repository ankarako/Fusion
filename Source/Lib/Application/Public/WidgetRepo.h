#ifndef	__APPLICATION_PUBLIC_WIDGETREPO_H__
#define __APPLICATION_PUBLIC_WIDGETREPO_H__

#include <Renderable.h>

#include <memory>
#include <vector>
#include <spimpl.h>

namespace fu {
namespace app {
///
class Widget;
///	\class WidgetRepo
///	\brief a simple repository that holds widgets
class WidgetRepo final : public Renderable
{
public:
	using widget_ptr_t = std::shared_ptr<Widget>;

	explicit WidgetRepo(const std::vector<widget_ptr_t>& widgets);

	void RegisterWidget(widget_ptr_t w);
	void UnregisterWidget(widget_ptr_t w);

	void Init() override { };
	virtual void Render() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class WidgetRepo
}	///	!namespace app
}	///	!anamesapce fu
#endif	///	!__APPLICATION_PUBLIC_WIDGETREPO_H__