#ifndef	__APPLICATION_PUBLIC_INPUTMANAGER_H__
#define __APPLICATION_PUBLIC_INPUTMANAGER_H__

#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace app {
namespace input {
///	\struct MousePos
///	\brief the mouse's position
struct MousePos
{
	double X{ 0.0 };
	double Y{ 0.0 };
	/// Construction
	///	\brief default constructor
	MousePos() = default;
	///	\brief construct from specified mouse position
	///	\param	x	the x-coordinate of the mouse position
	///	\param	y	the y-coordinate of the mouse position
	MousePos(double x, double y)
		: X(x), Y(y)
	{ }
};	///	!struct MousePos
///	\struct MouseScroll
///	\brief mouse scrolling data
struct MouseScroll
{
	double X{ 0.0 };
	double Y{ 0.0 };
	///	Construction
	///	\brief default constructor
	MouseScroll() = default;
	///	\brief construct from mouse scrolling offsets
	///	\param	x	the horizontal scroll amount
	///	\param	y	the vertical scroll amount
	MouseScroll(double x, double y)
		: X(x), Y(y)
	{ }
};	///	!struct MouseScroll
///	\enum MouseButton
///	\brief simple mouse bytton determination
enum class MouseButton : unsigned char
{
	Left,
	Right,
	Middle
};	///	!enum MouseButton
///	\class InputManager
///	\brief Manages input
class InputManager
{
public:
	///	Construction
	///	\brief default constructor
	InputManager();
	///	\brief get the mouse position
	///	\return the mouse position
	const MousePos& MousePosition() const;
	///	\brief get the pressed mouse buttons
	///	\return	the pressed mouse buttons
	const MouseButton& PressedMouseButtons() const;
	///	\brief set the mouse position
	///	\param	pos the mouse position to set
	void SetMousePosition(const MousePos& pos);
	///	\brief set pressed mouse buttons
	///	\param	button	the button that is pressed
	void SetMouseButtonPressed(MouseButton button);
	///	\brief set released mouse buttons
	///	\param	button the released mouse button
	void SetMouseButtonReleased(MouseButton button);
	///	\brief set the scrollo amount
	///	\param	scroll	the scroll amount to set
	void SetMouseScroll(MouseScroll scroll);
	/// events
	rxcpp::observable<MousePos>		OnMouseMove();
	rxcpp::observable<MouseButton>	OnMouseButtonPressed();
	rxcpp::observable<MouseButton>	OnMouseButtonReleased();
	rxcpp::observable<MouseScroll> 	OnMouseScroll();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class InputManager
}	///	!namespace input
}	///	!namespace app
#endif	///	!__APPLICATION_PUBLIC_INPUTMANAGER_H__