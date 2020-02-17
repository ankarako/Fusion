#ifndef __APPLICATION_PUBLIC_WINDOWFLAGS_H__
#define __APPLICATION_PUBLIC_WINDOWFLAGS_H__

namespace app {
///	\class WindowFlags
///	\brief simple window functionalities and data
class WindowFlags
{
public:
	///	\brief request to shutdown the window
	void RequestShutDown()
	{
		m_ShutdownFlag = true;
	}
	///	\brief poll the shutdown flag
	///	\return the value of the shutdown flag
	bool PollShutDownFlag() const
	{
		return m_ShutdownFlag;
	}
	///	\brief set the window's width
	///	\param	width	the width to set
	void SetWindowWidth(int width)
	{
		m_WindowWidth = width;
	}
	///	\brief get the window's width
	///	\return the window's width
	int GetWindowWidth() const
	{
		return m_WindowWidth;
	}
	///	\brief set the window's height
	///	\param	height	the window's height to set
	void SetWindowHeight(int height)
	{
		m_WindowHeight = height;
	}
	///	\brief get the window's height
	///	\return the window's height
	int GetWindowHeight() const
	{
		return m_WindowHeight;
	}
	///	\brief set the window frame buffer's width
	///	\param	width
	void SetFrameBufferWidth(int width)
	{
		m_WindowFrameBufferWidth = width;
	}
	///	\brief get the window's frame buffer width
	///	\return the frame buffer's width
	int GetFrameBufferWidth() const
	{
		return m_WindowFrameBufferWidth;
	}
	///	\brief set the frame buffer's height
	///	\param	height	the height to set
	void SetFrameBufferHeight(int height)
	{
		m_WindowFrameBufferHeight = height;
	}
	///	\brief get the fgrame buffer's height
	///	\return the frame buffer's height
	int GetFrameBufferHeight() const
	{
		return m_WindowFrameBufferHeight;
	}
private:
	bool	m_ShutdownFlag{ false };
	int 	m_WindowWidth{ 0 };
	int		m_WindowHeight{ 0 };
	int		m_WindowFrameBufferWidth{ 0 };
	int		m_WindowFrameBufferHeight{ 0 };
};	///	!class WindowFlags
}	///	!namespace app
#endif	///	!__APPLICATION_PUBLIC_WINDOWFLAGS_H__