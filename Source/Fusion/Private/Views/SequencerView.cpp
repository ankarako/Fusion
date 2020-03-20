#include <Views/SequencerView.h>
#include <imgui.h>
#include <string>
#include <vector>
#include <ImSequencer.h>
#include <FontDef.h>
#include <FontManager.h>
#include <imgui_internal.h>
#include <plog/Log.h>
#include <chrono>

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
	int GetFrameMin() const override
	{
		return m_FrameMin;
	}
	///	\brief get the sequence's max frame
	///	\return		the sequence's max frame
	int GetFrameMax() const override
	{
		return m_FrameMax;
	}
	/// \brief get the sequience's item count
	///	\return the sequence's item count
	int GetItemCount() const override
	{
		return m_SequenceItems.size();
	}
	///	\brief get the number of available types
	///	\return the count of the available SequenceItem types
	int GetItemTypeCount() const override
	{
		return static_cast<int>(SequenceItemType::Count);
	}
	///	\brief get the name of the specified item type
	///	\param	typeIndex	the index of the type
	///	\return		the name of the specified type
	const char* GetItemTypeName(int typeIndex) const override
	{
		const char* str = k_SequenceItemType2Str.at(static_cast<SequenceItemType>(typeIndex)).c_str();
		return str;
	}
	/// \brief get the label of the specified sequence item
	///	\param	index	the item's index
	///	\return	the lavel of the specified sequence item
	const char* GetItemLabel(int index) const override
	{
		char temps[512];
		std::memset(temps, 0, 512);
		std::string str = "[0" + std::to_string(index) + "] " + k_SequenceItemType2Str.at(m_SequenceItems[index].Type) + ": " + m_SequenceItems[index].Name;
		std::memcpy(temps, str.data(), str.size());
		return temps;
	}
	///	\brief get the specified sequence item 
	///	\param	index	the item's index
	///	\param[out] start	the item's start frame
	///	\param[out] end		the item's end frame
	///	\param[out] color	the item's color (for now hard coded)
	void Get(int index, int** start, int** end, int* type, unsigned int* color) override
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
		
	}

	void Add(const SequenceItem& item)
	{
		m_SequenceItems.push_back(item);
	}
	/// \brief delete a sequence item 
	///	\param index	the index of the item to delete
	void Del(int index) override
	{
		m_SequenceItems.erase(m_SequenceItems.begin() + index);
	}
	/// \brief duplicate an item
	///	\param	index	the index of the item to duplicate
	void Duplicate(int index) override
	{
		return m_SequenceItems.push_back(m_SequenceItems[index]);
	}
	///
	size_t GetCustomHeight(int index)
	{
		return m_SequenceItems[index].Expanded ? 100 : 0;
	}
	///	\brief expand the selected item
	///	\param	index	the item to expand
	void DoubleClick(int index) override
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
	/// \brief custom draw
	///	Draw custom ui when a sequencer item is expanded
	///	\param	index		the sequence item's index
	///	\param	drawList	current window's draw list
	///	\param	rc
	///	\param	legenRect
	///	\param	clippingRect
	///	\param	legendClippingrect
	void CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect) override
	{
		//auto& item = m_SequenceItems[index];
		//if (item.Type == SequenceItemType::Animation)
		//{
		//	/// legend
		//	ImVec2 pta(legendRect.Min.x, legendRect.Min.y);
		//	ImVec2 ptb(legendRect.Max.x, legendRect.Max.y);
		//	ImGui::SetCursorScreenPos(pta);
		//	ImGui::PushItemWidth(100.f);
		//	if (ImGui::SliderFloat("Translation", &trans, 0.0f, 10.0f))
		//	{

		//	}
		//	ImGui::PopItemWidth();
		//}
	}
	/// \brief Custom draw compact
	///	custom draw when an item is not expanded
	///	\param	index
	///	\param	rc
	///	\param	clippingRect
	void CustomDrawCompact(int index, ImDrawList* drawList, const ImRect& rc, const ImRect& clippingRect) override
	{

	}
	///	\brief get the specified item
	///	\param	index	the item's index to return
	///	\return	the specified item
	const SequenceItem& GetItem(int index)
	{
		return m_SequenceItems[index];
	}
	/// \brief BeginEdit
	void BeginEdit(int index) override
	{
		//LOG_DEBUG << "BeginEdit";
		
		
	}

	void BeginEdit(int index, int movingPart, int offset) override
	{
		auto& item = m_SequenceItems[index];
		m_CurrentItemEditState.EditMode = movingPart;
		m_CurrentItemEditState.BeginEditValue = offset;
	}

	void EndEdit() override
	{
		
	}

	void EndEdit(int index, int movingPart, int offset) override
	{
		m_CurrentItemEditState.EndEditValue = offset;
		int diff = m_CurrentItemEditState.EndEditValue - m_CurrentItemEditState.BeginEditValue;
		auto& item = m_SequenceItems[index];
		if (diff != 0)
		{
			if (m_CurrentItemEditState.EditMode == 1)
			{
				item.FrameStart += diff;
				LOG_DEBUG << "Expanded/Shrinked from start: "
					<< m_CurrentItemEditState.EndEditValue - m_CurrentItemEditState.BeginEditValue;
			}
			else if (m_CurrentItemEditState.EditMode == 2)
			{
				item.FrameEnd += diff;
				LOG_DEBUG << "Expanded/Shrinked from end: "
					<< m_CurrentItemEditState.EndEditValue - m_CurrentItemEditState.BeginEditValue;
			}
			else if (m_CurrentItemEditState.EditMode == 3)
			{
				item.SeqFrameStart += diff;
				item.SeqFrameEnd += diff;
				LOG_DEBUG << "Moved hole item frames: "
					<< m_CurrentItemEditState.EndEditValue - m_CurrentItemEditState.BeginEditValue;
			}
		}
	}
private:
	struct EditState
	{
		int EditMode{ 0 };
		int BeginEditValue{ 0 };
		int EndEditValue{ 0 };
	};
	float trans{0};
	int			m_FrameMin;
	int			m_FrameMax;
	EditState	m_CurrentItemEditState;
	std::vector<SequenceItem> m_SequenceItems;
};
///	\struct Impls
struct SequencerView::Impl
{
	using time_t = std::chrono::system_clock::time_point;

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
	bool			m_ShowProps{ false };
	bool			m_CursorMoved{ false };
	std::chrono::steady_clock::time_point	m_StartTime;
	std::chrono::nanoseconds				m_FramePeriod;
	
	rxcpp::composite_subscription			m_PlaybackLifetime;
	rxcpp::observable<long>					m_PlaybackObs;
	
	rxcpp::subjects::subject<SequenceItem>	m_SequeceItemFlowInSubj;
	rxcpp::subjects::subject<int>			m_CurrentFrameFlowInSubj;
	rxcpp::subjects::subject<void*>			m_OnPlayButtonClickedSubj;
	rxcpp::subjects::subject<void*>			m_OnPauseButtonClickedSubj;
	rxcpp::subjects::subject<void*>			m_OnStopButtonClickedSubj;
	rxcpp::subjects::subject<void*>			m_OnSeekForwardButtonClickedSubj;
	rxcpp::subjects::subject<void*>			m_OnSeekBackwardButtonClickedSubj;
	rxcpp::subjects::subject<int>			m_OnTransportPositionChangedSubj;

	rxcpp::subjects::subject<void*>			m_OnVideoStartPlaybackSubj;
	rxcpp::subjects::subject<void*>			m_OnAnimationStartPlaybackSubj;
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
			m_Sequence.Add(item);
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
		///
		double fps = 30.0;
		double periodSecs = 1.0 / fps;
		auto framePeriod =
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(periodSecs));
		m_FramePeriod = std::chrono::duration_cast<std::chrono::nanoseconds>(framePeriod);
		m_PlaybackObs = rxcpp::observable<>::interval(m_FramePeriod).as_dynamic();
	}
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
	int count = m_Impl->m_Sequence.GetItemCount();
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
			m_Impl->m_CurrentFrame--;
			for (int i = 0; i < count; i++)
			{
				auto& item = m_Impl->m_Sequence.GetItem(i);
				if (item.SeqFrameStart <= m_Impl->m_CurrentFrame)
				{
					item.OnSeekBackward.get_subscriber().on_next(nullptr);
				}
			}
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
				m_Impl->m_StartTime = std::chrono::high_resolution_clock::now();
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
				m_Impl->m_PlaybackLifetime.clear();
				for (int i = 0; i < count; i++)
				{
					auto& item = m_Impl->m_Sequence.GetItem(i);
					if (item.SeqFrameStart <= m_Impl->m_CurrentFrame && item.SeqFrameEnd >= m_Impl->m_CurrentFrame)
					{
						item.OnPause.get_subscriber().on_next(nullptr);
					}
				}
			}
		}
		if (m_Impl->m_State == Impl::SequencerState::Playing)
		{
			auto now = std::chrono::high_resolution_clock::now();
			auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_Impl->m_StartTime).count();
			if (dur >= m_Impl->m_FramePeriod.count())
			{
				m_Impl->m_CurrentFrame++;
				m_Impl->m_StartTime = now;
				for (int i = 0; i < count; i++)
				{
					auto& item = m_Impl->m_Sequence.GetItem(i);
					if (item.SeqFrameStart == m_Impl->m_CurrentFrame)
					{
						item.OnStartPlayback.get_subscriber().on_next(nullptr);
					}
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_STOP "##seq"))
		{
			m_Impl->m_State = Impl::SequencerState::Stopped;
			m_Impl->m_OnStopButtonClickedSubj.get_subscriber().on_next(nullptr);
			m_Impl->m_PlaybackLifetime.clear();
			for (int i = 0; i < count; i++)
			{
				auto& item = m_Impl->m_Sequence.GetItem(i);
				if (item.SeqFrameStart <= m_Impl->m_CurrentFrame && item.SeqFrameEnd >= m_Impl->m_CurrentFrame)
				{
					item.OnStop.get_subscriber().on_next(nullptr);
				}
			}
			m_Impl->m_CurrentFrame = 0;
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
			m_Impl->m_CurrentFrame++;
			for (int i = 0; i < count; i++)
			{
				auto& item = m_Impl->m_Sequence.GetItem(i);
				if (item.SeqFrameStart <= m_Impl->m_CurrentFrame && item.SeqFrameEnd >= m_Impl->m_CurrentFrame)
				{
					item.OnSeekForward.get_subscriber().on_next(nullptr);
				}
			}
		}
		if (m_Impl->m_State == Impl::SequencerState::Playing)
		{
			ImGui::PopItemFlag();
		}
		
		auto seqFlags = ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_CHANGE_FRAME;
		if (ImSequencer::Sequencer(&m_Impl->m_Sequence, &m_Impl->m_CurrentFrame, &m_Impl->m_Expanded, &m_Impl->m_SelectedEntry, &m_Impl->m_FirstFrame, &m_Impl->m_CursorMoved, seqFlags))
		{
			if (!m_Impl->m_CursorMoved)
			{
				LOG_DEBUG << "Moved";
				m_Impl->m_CursorMoved = false;
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
rxcpp::observable<void*> fu::fusion::SequencerView::OnVideoStartPlayback()
{
	return m_Impl->m_OnVideoStartPlaybackSubj.get_observable().as_dynamic();
}
rxcpp::observable<void*> fu::fusion::SequencerView::OnAnimationStartPlayback()
{
	return m_Impl->m_OnAnimationStartPlaybackSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu