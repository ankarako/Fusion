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
	
}
///	\brief load a media file
///	\param	filepath	the path of the file to load
void PlayerModel::LoadFile(const std::string& filepath)
{
	///	brief load 
	m_Impl->m_DecodingNode->LoadFile(filepath);
	size_t frameCount = m_Impl->m_DecodingNode->GetFrameCount();
	m_Impl->m_FrameCountFlowOutSubj.get_subscriber().on_next(frameCount);
}
///	\brief start playback
void PlayerModel::Start()
{
	double fps = m_Impl->m_DecodingNode->GetFrameRate();
	double period = 1 / fps; ///	period in seconds
	/// convert to chrono nanoseconds
	auto periodNano = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(period));
	auto playbackObs = rxcpp::observable<>::interval(periodNano, rxcpp::synchronize_new_thread());

	m_Impl->m_PlaybackLifetime.add(
		playbackObs.subscribe([this](auto _)
	{
		int curPos = m_Impl->m_DecodingNode->GetCurrentFramePosition();
		m_Impl->m_DecodingNode->GenerateFrame();
		m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(curPos);
		curPos++;
		m_Impl->m_DecodingNode->SetCurrentFramePos(curPos);
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