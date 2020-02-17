#include <InputManager.h>

namespace app {
namespace input {
///	\struct Impl
///	\brief InputManager implementation
struct InputManager::Impl
{
	MousePos	m_MousePos;
	MouseButton	m_MouseButton;

	rxcpp::subjects::subject<MousePos> 		m_OnMouseMoveSubject;
	rxcpp::subjects::subject<MouseButton>	m_OnMouseButtonPressedSubject;
	rxcpp::subjects::subject<MouseButton>	m_OnMouseButtonReleasedSubject;
	rxcpp::subjects::subject<MouseScroll>	m_OnMouseScrollSubject;
	/// Destruction
	~Impl()
	{
		m_OnMouseMoveSubject.get_subscriber().on_completed();
		m_OnMouseButtonPressedSubject.get_subscriber().on_completed();
		m_OnMouseButtonReleasedSubject.get_subscriber().on_completed();
		m_OnMouseScrollSubject.get_subscriber().on_completed();
	}
};	///	!struct Impl
///	Construction
///	\brief default constructor
InputManager::InputManager()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
///	\brief get the mouse position
///	\return the mouse position
const MousePos& InputManager::MousePosition() const
{
	return m_Impl->m_MousePos;
}
///	\brief get the pressed mouse buttons
///	\return	the pressed mouse buttons
const MouseButton& InputManager::PressedMouseButtons() const
{
	return m_Impl->m_MouseButton;
}
///	\brief set the mouse position
///	\param	pos the mouse position to set
void InputManager::SetMousePosition(const MousePos& pos)
{
	m_Impl->m_MousePos = pos;
	m_Impl->m_OnMouseMoveSubject.get_subscriber().on_next(m_Impl->m_MousePos);
}
///	\brief set pressed mouse buttons
///	\param	button	the button that is pressed
void InputManager::SetMouseButtonPressed(MouseButton button)
{
	m_Impl->m_MouseButton = (MouseButton)((unsigned char)m_Impl->m_MouseButton & (~(unsigned char)(button)));
	m_Impl->m_OnMouseButtonPressedSubject.get_subscriber().on_next(button);
}
///	\brief set released mouse buttons
///	\param	button the released mouse button
void InputManager::SetMouseButtonReleased(MouseButton button)
{
	m_Impl->m_MouseButton = (MouseButton)((unsigned char)m_Impl->m_MouseButton & (~(unsigned char)(button)));
	m_Impl->m_OnMouseButtonReleasedSubject.get_subscriber().on_next(button);
}
///	\brief set the scrollo amount
///	\param	scroll	the scroll amount to set
void InputManager::SetMouseScroll(MouseScroll scroll)
{
	m_Impl->m_OnMouseScrollSubject.get_subscriber().on_next(scroll);
}

rxcpp::observable<MousePos>		InputManager::OnMouseMove()
{
	return m_Impl->m_OnMouseMoveSubject.get_observable().as_dynamic();
}
rxcpp::observable<MouseButton>	InputManager::OnMouseButtonPressed()
{
	return m_Impl->m_OnMouseButtonPressedSubject.get_observable().as_dynamic();
}
rxcpp::observable<MouseButton>	InputManager::OnMouseButtonReleased()
{
	return m_Impl->m_OnMouseButtonReleasedSubject.get_observable().as_dynamic();
}
rxcpp::observable<MouseScroll> 	InputManager::OnMouseScroll()
{
	return m_Impl->m_OnMouseScrollSubject.get_observable().as_dynamic();
}
}	///	!namespace input
}	///	!namespace app