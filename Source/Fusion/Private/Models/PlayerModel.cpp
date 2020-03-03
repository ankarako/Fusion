#include <Models/PlayerModel.h>
#include <DecodingNode.h>
#include <plog/Log.h>

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
	///	
	rxcpp::subscription				m_PlaybackSubscription;
	std::atomic<bool>				m_ClockStarted{ false };
	std::atomic<State>				m_State{ State::Idle };
	std::chrono::milliseconds		m_ClockStart{ 0 };
	std::chrono::milliseconds		m_FramePeriod{ 0 };
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
	/// convert fps to period
	double fps = m_Impl->m_DecodingNode->GetFrameRate();
	auto d_millis = std::chrono::duration<double, std::milli>(1000.0 / fps);
	m_Impl->m_FramePeriod = std::chrono::duration_cast<std::chrono::milliseconds>(d_millis);
	m_Impl->m_State = Impl::State::Stopped;
	m_Impl->m_FrameCountFlowOutSubj.get_subscriber().on_next(frameCount);
}
///	\brief start playback
void PlayerModel::Start()
{
	if (m_Impl->m_State.load(std::memory_order_seq_cst) != Impl::State::Playing)
	{
		m_Impl->m_State.store(Impl::State::Playing, std::memory_order_seq_cst);
	}
	if (!m_Impl->m_ClockStarted.load(std::memory_order_seq_cst))
	{
		m_Impl->m_ClockStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
		m_Impl->m_ClockStarted.store(true, std::memory_order_seq_cst);
	}
	auto start = m_Impl->m_ClockStart.count();
	while (m_Impl->m_State.load(std::memory_order_seq_cst) == Impl::State::Playing)
	{	
		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		auto tick = now - start;
		auto period = m_Impl->m_FramePeriod.count();
		if (tick >= period)
		{
			m_Impl->m_DecodingNode->GenerateFrame();
			size_t curPos = m_Impl->m_DecodingNode->GetCurrentFramePosition();
			curPos++;
			m_Impl->m_DecodingNode->SetCurrentFramePos(curPos);
			m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(curPos);
			start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		else
		{
			start += tick;
		}
	}
	
}
///	\brief pause playback
void PlayerModel::Pause()
{
	if (m_Impl->m_ClockStarted.load(std::memory_order_seq_cst))
	{
		m_Impl->m_ClockStarted.store(false, std::memory_order_seq_cst);
		m_Impl->m_State.store(Impl::State::Paused, std::memory_order_seq_cst);
	}
		
}
///	\brief stio playback
void PlayerModel::Stop()
{
	if (m_Impl->m_ClockStarted.load(std::memory_order_seq_cst))
	{
		m_Impl->m_ClockStarted.store(false, std::memory_order_seq_cst);
		m_Impl->m_State.store(Impl::State::Stopped, std::memory_order_seq_cst);
		m_Impl->m_DecodingNode->SetCurrentFramePos(0);
		m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(m_Impl->m_DecodingNode->GetCurrentFramePosition());
		m_Impl->m_DecodingNode->GenerateFrame();
	}
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