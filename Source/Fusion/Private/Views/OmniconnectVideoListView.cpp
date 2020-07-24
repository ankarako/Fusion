#include <Views/OmniconnectVideoListView.h>
#include <imgui.h>

namespace fu {
namespace fusion {

struct OmniconnectVideoListView::Impl
{
	omni_video_list_ptr_t	m_VideoList;
	int						m_SelectedVideoIndex;
	rxcpp::subjects::subject<omni_video_list_ptr_t> m_VideoElementListFlowInSubj;
	rxcpp::subjects::subject<int>					m_DownloadSelectedIndexFlowOutSubj;

	Impl() { }
};	///	!struct Impl

OmniconnectVideoListView::OmniconnectVideoListView()
	: app::Widget("Omniconnect VideoList View"), m_Impl(spimpl::make_unique_impl<Impl>())
{ }

void OmniconnectVideoListView::Init()
{
	/// video list in
	m_Impl->m_VideoElementListFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const omni_video_list_ptr_t& list) { m_Impl->m_VideoList = list; });
}

void OmniconnectVideoListView::Render()
{
	ImGui::Begin("Omniconnect Video List");
	{
		for (int i = 0; i < m_Impl->m_VideoList->size(); ++i)
		{
			if (i != m_Impl->m_SelectedVideoIndex)
				m_Impl->m_VideoList->at(i).Selected = false;
		}
		for (int i = 0; i < m_Impl->m_VideoList->size(); ++i)
		{
			if(ImGui::RadioButton(m_Impl->m_VideoList->at(i).Name.c_str(), m_Impl->m_VideoList->at(i).Selected))
			{
				m_Impl->m_SelectedVideoIndex = i;

				if (m_Impl->m_VideoList->at(i).Selected)
					m_Impl->m_VideoList->at(i).Selected = false;
				else
					m_Impl->m_VideoList->at(i).Selected = true;
			}
		}
		ImGui::PushItemWidth(100.0f);
		if (ImGui::Button("Download##video"))
		{
			m_Impl->m_DownloadSelectedIndexFlowOutSubj.get_subscriber().on_next(m_Impl->m_SelectedVideoIndex);
			this->Deactivate();
		}
	}
	ImGui::End();
}

rxcpp::observer<omni_video_list_ptr_t> OmniconnectVideoListView::VideoListFlowIn()
{
	return m_Impl->m_VideoElementListFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<int> OmniconnectVideoListView::DownloadSelectedVideoIndexFlowOut()
{
	return m_Impl->m_DownloadSelectedIndexFlowOutSubj.get_observable().as_dynamic();
}


}	///	!namespace fusion
}	///	!namespace fu