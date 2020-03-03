#include <Models/PlayerModel.h>
#include <DecodingNode.h>
#include <plog/Log.h>
#include <rxcpp/rx-scheduler.hpp>

#include <chrono>
#include <atomic>

namespace fu {
namespace fusion {
///	\brief Video player model implementation
struct PlayerModel::Impl
{
	///	\rnum State
	///	\brief plaback state
	enum class State
	{
		Idle = 0,
		Playing,
		Paused,
		Stopped
	};	///	!enum State
	///	a decoding node used to decode video files
	trans::DecodingNode				m_DecodingNode{ nullptr };
	///
	rxcpp::observable<long>			m_PlaybackObs;
	/// playback subscription
	rxcpp::composite_subscription	m_PlaybackLifetime;
	/// current frame id output
	rxcpp::subjects::subject<size_t>	m_CurrentFrameIdFlowOutSubj;
	/// current frame count
	rxcpp::subjects::subject<size_t>	m_FrameCountFlowOutSubj;
	///	playback start time for correct frame rate
	std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	///	frame rate (actually frame period)
	std::chrono::milliseconds			m_FramePeriod;
	///	frame period tolerance for frame rate checking
	std::chrono::milliseconds			m_FramePeriodTolerance{ 20 };
	/// Construction
	/// \brief default constructor
	///	does nothing
	Impl() 
		: m_DecodingNode(trans::CreateDecodingNode())
	{ }
};
///	Construction
///	\brief default constructor
///	Initializes implementation struct
PlayerModel::PlayerModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
///	\brief initialize the player
///	Initializes the owned transcoding context
void PlayerModel::Init()
{

}
///	\brief destroy the player
///	Closes the MFSession
void PlayerModel::Destroy()
{
	m_Impl->m_DecodingNode->Release();
}
///	\brief load a media file
///	\param	filepath	the path of the file to load
void PlayerModel::LoadFile(const std::string& filepath)
{
	///	brief load 
	m_Impl->m_DecodingNode->LoadFile(filepath);
	size_t frameCount = m_Impl->m_DecodingNode->GetFrameCount();
	double fps = m_Impl->m_DecodingNode->GetFrameRate();
	double periodSecs = 1.0 / fps;
	m_Impl->m_FramePeriod = 
		std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(periodSecs));
	m_Impl->m_FrameCountFlowOutSubj.get_subscriber().on_next(frameCount);
}
///	\brief start playback
void PlayerModel::Start()
{
	auto playbackObs = rxcpp::observable<>::interval(m_Impl->m_FramePeriod, rxcpp::synchronize_new_thread());
	m_Impl->m_Start = std::chrono::high_resolution_clock::now();
	m_Impl->m_PlaybackLifetime.add(
		playbackObs.subscribe([this](auto _)
	{
		auto playbackNow = std::chrono::high_resolution_clock::now();
		std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(playbackNow - m_Impl->m_Start);
		/*if (diff >= m_Impl->m_FramePeriod - m_Impl->m_FramePeriodTolerance)
		{*/
			LOG_DEBUG << "Playback: " << diff.count();
			int curPos = m_Impl->m_DecodingNode->GetCurrentFramePosition();
			m_Impl->m_DecodingNode->GenerateFrame();
			m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(curPos);
			curPos++;
			m_Impl->m_DecodingNode->SetCurrentFramePos(curPos);
			m_Impl->m_Start = std::chrono::high_resolution_clock::now();
		//}
	}));
}
///	\brief pause playback
void PlayerModel::Pause()
{
	m_Impl->m_PlaybackLifetime.clear();
}
///	\brief stio playback
void PlayerModel::Stop()
{
	m_Impl->m_PlaybackLifetime.clear();
	m_Impl->m_DecodingNode->SetCurrentFramePos(0);
	m_Impl->m_DecodingNode->GenerateFrame();
	m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(0);
}
///	\brief current frame id output
rxcpp::observable<size_t> fu::fusion::PlayerModel::CurrentFrameIdFlowOut()
{
	return m_Impl->m_CurrentFrameIdFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<size_t> fu::fusion::PlayerModel::FrameCountFlowOut()
{
	return m_Impl->m_FrameCountFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu