#include <Views/PlayerControllerView.h>
#include <FontManager.h>
#include <FontDef.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief PlayerControllerView implementation
struct PlayerControllerView::Impl
{
	enum class ControllerState
	{
		Idle = 0,
		Playing,
		Paused,
		Stopped,
		SeekForw,
		SeekBack
	};
	const ImVec2 k_WindowSize{ 200.0f, 50.0f };
	ControllerState m_State{ ControllerState::Idle };
	fman_ptr_t		m_FontManger;
	int				m_SliderValue{ 0 };
	int				m_SliderValueMax{ 0 };

	rxcpp::subjects::subject<void*> m_OnSeekBackwardButtonClickedSubj;
	rxcpp::subjects::subject<void*> m_OnPlayButtonClickedSubj;
	rxcpp::subjects::subject<void*> m_OnPauseButtonClickedSubj;
	rxcpp::subjects::subject<void*> m_OnStopButtonClickedSubj;
	rxcpp::subjects::subject<void*> m_OnSeekForwardButtonClickedSubj;
	rxcpp::subjects::subject<int>	m_OnSliderValueChangedSubj;
	rxcpp::subjects::subject<int>	m_FrameIdFlowIn;
	/// Construction
	Impl(fman_ptr_t fman) 
		: m_FontManger(fman)
	{ }
};	///	!struct Impl
/// Construction
PlayerControllerView::PlayerControllerView(fman_ptr_t fman)
	: app::Widget("PlayerController"), m_Impl(spimpl::make_unique_impl<Impl>(fman))
{ }

void PlayerControllerView::Render()
{
	bool isActive = this->IsActive();
	if (!isActive)
		return;

	bool checkPlayButton =
		m_Impl->m_State == Impl::ControllerState::Idle		||
		m_Impl->m_State == Impl::ControllerState::Paused	||
		m_Impl->m_State == Impl::ControllerState::Stopped	||
		m_Impl->m_State == Impl::ControllerState::SeekBack	||
		m_Impl->m_State == Impl::ControllerState::SeekForw;
	
	
	ImGui::SetWindowSize(m_Impl->k_WindowSize);
	ImGui::PushFont(m_Impl->m_FontManger->GetFont(app::FontManager::FontType::Regular));
	ImGui::Begin("Player", &isActive);
	{
		/// state is playing the user cannot seek
		if (m_Impl->m_State == Impl::ControllerState::Playing)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		}
		if (ImGui::Button(ICON_MD_SKIP_PREVIOUS))
		{
			m_Impl->m_State = Impl::ControllerState::SeekBack;
			m_Impl->m_OnSeekBackwardButtonClickedSubj.get_subscriber().on_next(nullptr);
		}
		if (m_Impl->m_State == Impl::ControllerState::Playing)
		{
			ImGui::PopItemFlag();
		}
		ImGui::SameLine();
		/// active playback button according to controller's state
		if (checkPlayButton)
		{
			if (ImGui::Button(ICON_MD_PLAY_ARROW))
			{
				m_Impl->m_State = Impl::ControllerState::Playing;
				m_Impl->m_OnPlayButtonClickedSubj.get_subscriber().on_next(nullptr);
			}
		}
		else
		{
			if (ImGui::Button(ICON_MD_PAUSE))
			{
				m_Impl->m_State = Impl::ControllerState::Paused;
				m_Impl->m_OnPauseButtonClickedSubj.get_subscriber().on_next(nullptr);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_STOP))
		{
			m_Impl->m_State = Impl::ControllerState::Stopped;
			m_Impl->m_OnStopButtonClickedSubj.get_subscriber().on_next(nullptr);
		}
		ImGui::SameLine();
		/// state is playing the user cannot seek
		if (m_Impl->m_State == Impl::ControllerState::Playing)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		}
		if (ImGui::Button(ICON_MD_SKIP_NEXT))
		{
			m_Impl->m_State = Impl::ControllerState::SeekForw;
			m_Impl->m_OnSeekForwardButtonClickedSubj.get_subscriber().on_next(nullptr);
		}
		if (m_Impl->m_State == Impl::ControllerState::Playing)
		{
			ImGui::PopItemFlag();
		}
		ImGui::SameLine();
		if (ImGui::SliderInt("##playbackslider", &m_Impl->m_SliderValue, 0, m_Impl->m_SliderValueMax))
		{
			m_Impl->m_OnSliderValueChangedSubj.get_subscriber().on_next(m_Impl->m_SliderValue);
		}
		
	}
	ImGui::End();
	ImGui::PopFont();
}

void PlayerControllerView::SetSliderValue(int value)
{
	m_Impl->m_SliderValue = value;
}

void PlayerControllerView::SetMaxSliderValue(int value)
{
	m_Impl->m_SliderValueMax = value;
}

rxcpp::observable<void*> fusion::PlayerControllerView::OnSeekBackwardButtonClicked()
{
	return m_Impl->m_OnSeekBackwardButtonClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fu::fusion::PlayerControllerView::OnPauseButtonClicked()
{
	return m_Impl->m_OnPauseButtonClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fusion::PlayerControllerView::OnPlayButtonClicked()
{
	return m_Impl->m_OnPlayButtonClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fusion::PlayerControllerView::OnStopButtonClicked()
{
	return m_Impl->m_OnStopButtonClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fusion::PlayerControllerView::OnSeekForwardButtonClicked()
{
	return m_Impl->m_OnSeekForwardButtonClickedSubj.get_observable().as_dynamic();
}

rxcpp::observer<int> fusion::PlayerControllerView::FrameIdFlowIn()
{
	return m_Impl->m_FrameIdFlowIn.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observable<int> fu::fusion::PlayerControllerView::OnSliderValueChanged()
{
	return m_Impl->m_OnSliderValueChangedSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}