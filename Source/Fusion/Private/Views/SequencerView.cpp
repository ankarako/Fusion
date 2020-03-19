#include <Views/SequencerView.h>
#include <imgui.h>
#include <string>
#include <vector>
#include <ImSequencer.h>
#include <FontDef.h>
#include <FontManager.h>
#include <imgui_internal.h>
#include <plog/Log.h>

namespace  fu {
namespace fusion {

///	\struct FusionSequence
///	\brief Implements ImSequencer interface
struct FusionSequence : public ImSequencer::SequenceInterface
{
	/// Construction
	FusionSequence()
		: m_FrameMin(0), m_FrameMax(0)
	{ }
	/// \brief set sequence's min frame
	///	\param	min	the min frame to set
	void SetFrameMin(int min)
	{
		m_FrameMin = min;
	}
	///	\brief set sequences max frame
	///	\param	max	the max frame to set
	void SetFrameMax(int max)
	{
		m_FrameMax = max;
	}
	///	\brief get the sequence's min frame
	///	\return		the sequence's min frame
	int GetFrameMin() const
	{
		return m_FrameMin;
	}
	///	\brief get the sequence's max frame
	///	\return		the sequence's max frame
	int GetFrameMax() const
	{
		return m_FrameMax;
	}
	/// \brief get the sequience's item count
	///	\return the sequence's item count
	int GetItemCount() const
	{
		return m_SequenceItems.size();
	}
	///	\brief get the number of available types
	///	\return the count of the available SequenceItem types
	int GetItemTypeCount() const
	{
		return static_cast<int>(SequenceItemType::Count);
	}
	///	\brief get the name of the specified item type
	///	\param	typeIndex	the index of the type
	///	\return		the name of the specified type
	const char* GetItemTypeName(int typeIndex) const
	{
		return k_SequenceItemType2Str.at(static_cast<SequenceItemType>(typeIndex));
	}
	/// \brief get the label of the specified sequence item
	///	\param	index	the item's index
	///	\return	the lavel of the specified sequence item
	const char* GetItemLabel(int index) const
	{
		char temps[512];
		sprintf_s(temps, "[%02d] %s", index, k_SequenceItemType2Str.at(m_SequenceItems[index].Type));
		return temps;
	}
	///	\brief get the specified sequence item 
	///	\param	index	the item's index
	///	\param[out] start	the item's start frame
	///	\param[out] end		the item's end frame
	///	\param[out] color	the item's color (for now hard coded)
	void Get(int index, int** start, int** end, int* type, unsigned int* color)
	{
		SequenceItem& item = m_SequenceItems[index];
		if (color)
			*color = 0xFFAA8080;
		if (start)
			*start = &item.FrameStart;
		if (end)
			*end = &item.FrameEnd;
		if (type)
			*type = static_cast<int>(item.Type);
	}
	///	\brief add a sequence item
	///	\param	type	the type of item to add
	///	\param	start	the starting frame of the item to add
	///	\param	end		the end frame of the item to add
	void Add(int type, int start, int end)
	{
		m_SequenceItems.push_back(SequenceItem{ static_cast<SequenceItemType>(type), start, end, false });
	}
	/// \brief delete a sequence item 
	///	\param index	the index of the item to delete
	void Del(int index)
	{
		m_SequenceItems.erase(m_SequenceItems.begin() + index);
	}
	/// \brief duplicate an item
	///	\param	index	the index of the item to duplicate
	void Duplicate(int index)
	{
		return m_SequenceItems.push_back(m_SequenceItems[index]);
	}
	///
	size_t GetCustomHeight(int index)
	{
		return m_SequenceItems[index].Expanded ? 100 : 0;
	}
	///
	void DoubleClick(int index)
	{
		if (m_SequenceItems[index].Expanded)
		{
			m_SequenceItems[index].Expanded = false;
			return;
		}
		for (auto& item : m_SequenceItems)
		{
			item.Expanded = false;
		}
		m_SequenceItems[index].Expanded = !m_SequenceItems[index].Expanded;
	}
	///
	void CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect)
	{

	}

	const SequenceItem& GetItem(int index)
	{
		return m_SequenceItems[index];
	}
private:
	int m_FrameMin;
	int m_FrameMax;
	std::vector<SequenceItem> m_SequenceItems;
};
///	\struct Impls
struct SequencerView::Impl
{
	enum class SequencerState
	{
		Idle = 0,
		Playing,
		Paused,
		Stopped,
		SeekForw,
		SeekBack
	};

	fman_ptr_t		m_FontManager;
	SequencerState	m_State{ SequencerState::Idle };
	FusionSequence	m_Sequence;
	int				m_CurrentFrame{ 0 };
	int				m_CurrentCursorFrame{ 0 };
	int				m_FirstFrame{ 0 };
	int				m_SelectedEntry{ -1 };
	bool			m_Expanded{ true };
	
	
	rxcpp::subjects::subject<SequenceItem>	m_SequeceItemFlowInSubj;
	rxcpp::subjects::subject<int>			m_CurrentFrameFlowInSubj;
	rxcpp::subjects::subject<void*>			m_OnPlayButtonClickedSubj;
	rxcpp::subjects::subject<void*>			m_OnPauseButtonClickedSubj;
	rxcpp::subjects::subject<void*>			m_OnStopButtonClickedSubj;
	rxcpp::subjects::subject<void*>			m_OnSeekForwardButtonClickedSubj;
	rxcpp::subjects::subject<void*>			m_OnSeekBackwardButtonClickedSubj;
	rxcpp::subjects::subject<int>			m_OnTransportPositionChangedSubj;
	/// Construction
	Impl(fman_ptr_t fman)
		: m_FontManager(fman)
	{ 
		m_Sequence.SetFrameMin(0);
		m_Sequence.SetFrameMax(100);
		///====================
		/// SequenceItemFlowIn
		///====================
		m_SequeceItemFlowInSubj.get_observable().as_dynamic()
			.subscribe([this](const SequenceItem& item) 
		{
			m_Sequence.Add(static_cast<int>(item.Type), item.FrameStart, item.FrameEnd);
			m_Sequence.SetFrameMax(item.FrameEnd);
		});
		///=======================
		///	Current FRame Flow In
		///=======================
		m_CurrentFrameFlowInSubj.get_observable().as_dynamic()
			.subscribe([this](int index) 
		{
			m_CurrentFrame = index;
		});
	}
	///

};	///	!struct Impl
/// Construction
SequencerView::SequencerView(fman_ptr_t fman)
	: app::Widget("Sequencer"), m_Impl(spimpl::make_unique_impl<Impl>(fman))
{ }

void SequencerView::Render()
{
	bool isActive = this->IsActive();
	if (!isActive)
		return;

	bool checkPlayButton =
		m_Impl->m_State == Impl::SequencerState::Idle		||
		m_Impl->m_State == Impl::SequencerState::Paused		||
		m_Impl->m_State == Impl::SequencerState::Stopped	||
		m_Impl->m_State == Impl::SequencerState::SeekBack	||
		m_Impl->m_State == Impl::SequencerState::SeekForw;
	
	ImGui::PushFont(m_Impl->m_FontManager->GetFont(app::FontManager::FontType::Regular));
	auto flags = ImGuiWindowFlags_HorizontalScrollbar;
	ImGui::Begin("Sequencer", &isActive, flags);
	{
		if (m_Impl->m_State == Impl::SequencerState::Playing)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		}
		if (ImGui::Button(ICON_MD_SKIP_PREVIOUS "##seq"))
		{
			m_Impl->m_State = Impl::SequencerState::SeekBack;
			m_Impl->m_OnSeekBackwardButtonClickedSubj.get_subscriber().on_next(nullptr);
		}
		if (m_Impl->m_State == Impl::SequencerState::Playing)
		{
			ImGui::PopItemFlag();
		}
		ImGui::SameLine();
		if (checkPlayButton)
		{
			if (ImGui::Button(ICON_MD_PLAY_ARROW "##seq"))
			{
				m_Impl->m_State = Impl::SequencerState::Playing;
				m_Impl->m_OnPlayButtonClickedSubj.get_subscriber().on_next(nullptr);
			}
		}
		else
		{
			if (ImGui::Button(ICON_MD_PAUSE "##seq"))
			{
				m_Impl->m_State = Impl::SequencerState::Paused;
				m_Impl->m_OnPauseButtonClickedSubj.get_subscriber().on_next(nullptr);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_STOP "##seq"))
		{
			m_Impl->m_State = Impl::SequencerState::Stopped;
			m_Impl->m_OnStopButtonClickedSubj.get_subscriber().on_next(nullptr);
		}
		ImGui::SameLine();
		if (m_Impl->m_State == Impl::SequencerState::Playing)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		}
		if (ImGui::Button(ICON_MD_SKIP_NEXT "##seq"))
		{
			m_Impl->m_State = Impl::SequencerState::SeekForw;
			m_Impl->m_OnSeekForwardButtonClickedSubj.get_subscriber().on_next(nullptr);
		}
		if (m_Impl->m_State == Impl::SequencerState::Playing)
		{
			ImGui::PopItemFlag();
		}
		
		auto seqFlags =
			ImSequencer::SEQUENCER_EDIT_STARTEND |
			//ImSequencer::SEQUENCER_ADD |
			//ImSequencer::SEQUENCER_DEL |
			//ImSequencer::SEQUENCER_COPYPASTE |
			ImSequencer::SEQUENCER_CHANGE_FRAME;
		if (ImSequencer::Sequencer(&m_Impl->m_Sequence, &m_Impl->m_CurrentFrame, &m_Impl->m_Expanded, &m_Impl->m_SelectedEntry, &m_Impl->m_FirstFrame, seqFlags))
		{
			LOG_DEBUG << "CurrentFrame : " << m_Impl->m_CurrentFrame;
			LOG_DEBUG << "SelectedEntry: " << m_Impl->m_SelectedEntry;
			LOG_DEBUG << "FirstFrame   : " << m_Impl->m_FirstFrame;
			std::string exp = m_Impl->m_Expanded ? "true" : "false";
			LOG_DEBUG << "Expanded     : " << exp;
			if (m_Impl->m_SelectedEntry >= 0)
			{
				LOG_DEBUG << "Selected Entry:";
				LOG_DEBUG << "\tFrame Start: " << m_Impl->m_Sequence.GetItem(m_Impl->m_SelectedEntry).FrameStart;
				LOG_DEBUG << "\tFrame End  : " << m_Impl->m_Sequence.GetItem(m_Impl->m_SelectedEntry).FrameEnd;
			}
		}
	}
	ImGui::End();
	ImGui::PopFont();
}

rxcpp::observer<SequenceItem> fu::fusion::SequencerView::SequencerItemFlowIn()
{
	return m_Impl->m_SequeceItemFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<int> fu::fusion::SequencerView::CurrentFrameFlowIn()
{
	return m_Impl->m_CurrentFrameFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<void*> fu::fusion::SequencerView::OnPlayButtonClicked()
{
	return m_Impl->m_OnPlayButtonClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fu::fusion::SequencerView::OnPauseButtonClicked()
{
	return m_Impl->m_OnPauseButtonClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fu::fusion::SequencerView::OnStopButtonClicked()
{
	return m_Impl->m_OnStopButtonClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fu::fusion::SequencerView::OnSeekBackwardButtonClicked()
{
	return m_Impl->m_OnSeekBackwardButtonClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> fu::fusion::SequencerView::OnSeekForwardButtonClicked()
{
	return m_Impl->m_OnSeekForwardButtonClickedSubj.get_observable().as_dynamic();
}

rxcpp::observable<int> fu::fusion::SequencerView::OnTransportPositionChanged()
{
	return m_Impl->m_OnTransportPositionChangedSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu