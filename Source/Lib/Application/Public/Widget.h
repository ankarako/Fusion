#ifndef	__APPLICATION_PUBLIC_WIDGET_H__
#define __APPLICATION_PUBLIC_WIDGET_H__

#include <rxcpp/rx.hpp>
#include <Activatable.h>

#include <string>

namespace fu {
namespace app {
///	\class Widget
///	\brief simple widget class
class Widget : public Activatable
{
public:
	/// Construction
	Widget(const std::string& name)
		: m_WidgetName(name)
	{ }

	const std::string& GetName()
	{
		return m_WidgetName;
	}

	virtual void Render() = 0;
	virtual ~Widget() = default;
private:
	Widget(const Widget& other) = delete;
	Widget(Widget&& other) = delete;
	Widget& operator=(const Widget& other) = delete;
	Widget& operator=(Widget&& other) = delete;

	std::string m_WidgetName;
};	///	class widget
}	///	!namespace app
}	///	!namespace fu
#endif	///	!__APPLICATION_PUBLIC_WIDGET_H__