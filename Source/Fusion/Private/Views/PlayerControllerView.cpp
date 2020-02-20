#include <Views/PlayerControllerView.h>
#include <FontManager.h>
#include <FontDef.h>
#include <imgui.h>

namespace fusion {
///	\struct Impl
///	\brief PlayerControllerView implementation
struct PlayerControllerView::Impl
{
	fman_ptr_t	m_FontManger;
	int m_SliderValue{ 0 };

	rxcpp::subjects::subject<void*> m_OnSeekBackwardButtonClickedSubj;
	rxcpp::subjects::subject<void*> m_OnPlayButtonClickedSubj;
	rxcpp::subjects::subject<void*> m_OnStopButtonClickedSubj;
	rxcpp::subjects::subject<void*> m_OnSeekForwardButtonClickedSubj;
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
	ImGui::PushFont(m_Impl->m_FontManger->GetFont(app::FontManager::FontType::Regular));
	ImGui::Begin("Player");
	{
		if (ImGui::Button(ICON_MD_SKIP_PREVIOUS))
		{
			m_Impl->m_OnSeekBackwardButtonClickedSubj.get_subscriber().on_next(nullptr);
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_PLAY_ARROW))
		{
			m_Impl->m_OnPlayButtonClickedSubj.get_subscriber().on_next(nullptr);
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_STOP))
		{
			m_Impl->m_OnStopButtonClickedSubj.get_subscriber().on_next(nullptr);
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_SKIP_NEXT))
		{
			m_Impl->m_OnSeekForwardButtonClickedSubj.get_subscriber().on_next(nullptr);
		}
		ImGui::SameLine();
		if (ImGui::SliderInt("##playbackslider", &m_Impl->m_SliderValue, 0, 10000))
		{

		}
		
	}
	ImGui::End();
	ImGui::PopFont();
}

rxcpp::observable<void*> fusion::PlayerControllerView::OnSeekBackwardButtonClicked()
{
	return m_Impl->m_OnSeekBackwardButtonClickedSubj.get_observable().as_dynamic();
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
}	///	!namespace fusion