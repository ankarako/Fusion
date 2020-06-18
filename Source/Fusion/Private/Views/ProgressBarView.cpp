#include <Views/ProgressBarView.h>
#include <imgui.h>

namespace fu {
namespace fusion {

struct ProgressBarView::Impl
{
	ImVec2		m_WindowPosition{ 0.f, 20.f };
	ImVec2		m_WindowSize{ 400.0f, 100.0f };
	ImVec2		m_ProgressBarPosition;
	ImVec2		m_ProgressBarSize;
	ImVec2		m_ProgressBarDist{ 40.f ,30.f };
	ImVec2		m_TextSize{ 0.0f, 0.0f };
	float		m_Progress{ 0.0f };

	std::string m_Message{ "" };

	bool m_SetWindowPos{ true };
	bool m_SetWindowSize{ true };
	bool m_TitleBarHeight;

	rxcpp::subjects::subject<pos_t>			m_PositionFlowInSubj;
	rxcpp::subjects::subject<float>			m_ProgressFlowInSubj;
	rxcpp::subjects::subject<std::string>	m_MessageFlowInSubj;
	///	\brief render the bar's background
	void RenderBarBg()
	{
		ImGui::GetWindowDrawList()->AddRectFilled(
			m_ProgressBarPosition,
			ImVec2(m_ProgressBarPosition.x + m_ProgressBarSize.x, m_ProgressBarPosition.y + m_ProgressBarSize.y),
			ImColor(0.039f, 0.027, 0.035, 0.8f));
	}
	/// \brief render the bar's progress
	void RenderBarProgress()
	{
		int progressSize = static_cast<int>(m_Progress * m_ProgressBarSize.x);
		ImGui::GetWindowDrawList()->AddRectFilled(
			m_ProgressBarPosition,
			ImVec2(m_ProgressBarPosition.x + progressSize, m_ProgressBarPosition.y + m_ProgressBarSize.y),
			ImColor(1.0f, 0.635f, 0.2f, 1.0f));
	}
	///	\brief set the bar's position
	void SetProgressBarPosition()
	{
		m_ProgressBarPosition.x = m_WindowPosition.x + m_ProgressBarDist.x;
		m_ProgressBarPosition.y = m_WindowPosition.y + m_ProgressBarDist.y + m_TitleBarHeight * 0.5f + m_TextSize.y;
	}
	/// \brief set the progress bar's size
	void SetProgressBarSize()
	{
		m_ProgressBarSize.x = m_WindowSize.x - 2 * m_ProgressBarDist.x;
		m_ProgressBarSize.y = m_WindowSize.y - 2 * m_ProgressBarDist.y - m_TextSize.y;
	}

	///	\brief set the window size
	void SetWindowSize(float w, float h)
	{
		m_WindowSize = ImVec2(w, h);
		SetProgressBarSize();
		m_SetWindowSize = true;
	}

	const bool WindowMoved()
	{
		ImVec2 cur = ImGui::GetWindowPos();
		return (cur.x != m_WindowPosition.x) || (cur.y != m_WindowPosition.y);
	}

	const bool WindowSizeChanged()
	{
		ImVec2 cur = ImGui::GetWindowSize();
		return (cur.x != m_WindowSize.x) || (cur.y != m_WindowSize.y);
	}

	Impl() 
	{
		/// Set the bar's size
		m_PositionFlowInSubj.get_observable().as_dynamic()
			.subscribe([this](const pos_t& pos) 
		{
			m_WindowPosition = ImVec2(pos[0], pos[1]);
			SetProgressBarPosition();
			m_SetWindowPos = true;
		});
		/// set the bar's message
		m_MessageFlowInSubj.get_observable().as_dynamic()
			.subscribe([this](const std::string& msg) 
		{
			m_Message = msg;
		});
		/// set the progress
		m_ProgressFlowInSubj.get_observable().as_dynamic()
			.subscribe([this](float prog) 
		{
			m_Progress = prog;
		});

	}
};

ProgressBarView::ProgressBarView()
	: app::Widget("Progress Bar"), m_Impl(spimpl::make_unique_impl<Impl>())
{ }

void ProgressBarView::Render()
{
	m_Impl->m_TextSize = ImGui::CalcTextSize(m_Impl->m_Message.c_str());
	ImGui::Begin("Progress##ProgressBar");
	{
		if (m_Impl->m_SetWindowPos)
		{
			m_Impl->m_TitleBarHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			ImGui::SetWindowPos(m_Impl->m_WindowPosition);
			m_Impl->m_SetWindowPos = false;
		}
		if (m_Impl->m_SetWindowSize)
		{
			ImGui::SetWindowSize(m_Impl->m_WindowSize);
			m_Impl->SetProgressBarSize();
			m_Impl->m_SetWindowSize = false;
		}
		if (m_Impl->WindowMoved())
		{
			m_Impl->m_WindowPosition = ImGui::GetWindowPos();
			m_Impl->m_SetWindowPos = true;
			m_Impl->SetProgressBarPosition();
		}
		if (m_Impl->WindowSizeChanged())
		{
			m_Impl->m_WindowSize = ImGui::GetWindowSize();
			m_Impl->SetProgressBarSize();
		}
		ImGui::Text(m_Impl->m_Message.c_str());
		m_Impl->RenderBarBg();
		m_Impl->RenderBarProgress();
	}
	ImGui::End();
}
rxcpp::observer<ProgressBarView::pos_t> ProgressBarView::PositionFlowIn()
{
	return m_Impl->m_PositionFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<float> ProgressBarView::ProgressFlowIn()
{
	return m_Impl->m_ProgressFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<std::string> ProgressBarView::MessageFlowIn()
{
	return m_Impl->m_MessageFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu