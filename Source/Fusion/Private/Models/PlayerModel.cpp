#include <Models/PlayerModel.h>
#include <Core/Coordination.h>
#include <DecodingNode.h>
#include <Core/SettingsRepo.h>
#include <Settings/PlayerSettings.h>
#include <plog/Log.h>
#include <rxcpp/rx-scheduler.hpp>
#include <chrono>
#include <atomic>
#include <deque>

namespace fu {
namespace fusion {
///	\brief Video player model implementation
struct PlayerModel::Impl
{
	using settings_ptr_t = std::shared_ptr<PlayerSettings>;
	/// coordination
	coord_ptr_t						m_Coord;
	///	a decoding node used to decode video files
	trans::DecodingNode				m_DecodingNode{ nullptr };
	///
	rxcpp::observable<long>			m_PlaybackObs;
	/// playback subscription
	rxcpp::composite_subscription	m_PlaybackLifetime;
	///	atomic flags for signifying playbck and frame generation
	std::atomic_bool				m_IsPlaying;
	std::atomic_bool				m_IsGenerating;
	/// Our settingss
	srepo_ptr_t						m_SettingsRepo;
	settings_ptr_t					m_Settings;
	///	playback start time for correct frame rate
	std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	///	frame rate (actually frame period)
	std::chrono::milliseconds			m_FramePeriod;
	///	frame period tolerance for frame rate checking
	std::chrono::milliseconds			m_FramePeriodTolerance{ 20 };
	/// current frame id output
	rxcpp::subjects::subject<size_t>	m_CurrentFrameIdFlowOutSubj;
	/// current frame count
	rxcpp::subjects::subject<size_t>	m_FrameCountFlowOutSubj;
	///	frame width output
	rxcpp::subjects::subject<int>		m_FrameWidthFlowOutSubj;
	///	frame height output
	rxcpp::subjects::subject<int>		m_FrameHeightFlowOutSubj;
	/// frame size output
	rxcpp::subjects::subject<uint2>		m_FrameSizeFlowOutSubj;
	///	current frame output
	rxcpp::subjects::subject<frame_t>	m_FrameFlowOutSubj;
	/// framme FIFO queue	
	std::deque<frame_t>					m_FrameQueue;
	/// Construction
	/// \brief default constructor
	///	does nothing
	Impl(coord_ptr_t coord, srepo_ptr_t srepo) 
		: m_Coord(coord), m_SettingsRepo(srepo), m_DecodingNode(trans::CreateDecodingNode())
	{ 
		m_IsPlaying.store(false, std::memory_order_seq_cst);
		m_IsGenerating.store(false, std::memory_order_seq_cst);
	}
};
///	Construction
///	\brief default constructor
///	Initializes implementation struct
PlayerModel::PlayerModel(coord_ptr_t coord, srepo_ptr_t srepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(coord, srepo))
{ }
///	\brief initialize the player
///	Initializes the owned transcoding context
void PlayerModel::Init()
{
	/// tedious
	/*m_Impl->m_DecodingNode->FrameFlowOut()
		.subscribe([this](frame_t& frame)
	{
		m_Impl->m_FrameQueue.emplace_back(frame);
	});*/
	/// create out settings
	m_Impl->m_Settings = std::make_shared<PlayerSettings>();
	/// register them to the repo
	m_Impl->m_SettingsRepo->RegisterSettings(m_Impl->m_Settings);
}
///	\brief destroy the player
///	Closes the MFSession
void PlayerModel::Destroy()
{
	m_Impl->m_PlaybackLifetime.unsubscribe();
	m_Impl->m_DecodingNode->Release();
}
///	\brief load a media file
///	\param	filepath	the path of the file to load
void PlayerModel::LoadFile(const std::string& filepath)
{
	/// load 
	m_Impl->m_DecodingNode->LoadFile(filepath);
	size_t frameCount = m_Impl->m_DecodingNode->GetFrameCount();
	double fps = m_Impl->m_DecodingNode->GetFrameRate();
	double periodSecs = 1.0 / fps;
	m_Impl->m_FramePeriod = 
		std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(periodSecs));
	m_Impl->m_FrameCountFlowOutSubj.get_subscriber().on_next(frameCount);
	m_Impl->m_FrameWidthFlowOutSubj.get_subscriber().on_next(m_Impl->m_DecodingNode->GetFrameWidth());
	m_Impl->m_FrameHeightFlowOutSubj.get_subscriber().on_next(m_Impl->m_DecodingNode->GetFrameHeight());
	m_Impl->m_FrameSizeFlowOutSubj.get_subscriber().on_next(make_uint2(m_Impl->m_DecodingNode->GetFrameWidth(), m_Impl->m_DecodingNode->GetFrameHeight()));
	m_Impl->m_Settings->LoadedVideoFilepath = filepath;
	/// create the interval observable according to the frame rate
	std::chrono::nanoseconds periodNano = std::chrono::duration_cast<std::chrono::nanoseconds>(m_Impl->m_FramePeriod);
	m_Impl->m_PlaybackObs = rxcpp::observable<>::interval(periodNano).as_dynamic();
}
///	\brief start playback
void PlayerModel::Start()
{
	/// Play one frame task
	m_Impl->m_PlaybackLifetime.add(
		m_Impl->m_PlaybackObs.subscribe(
			[this](auto _)
	{
		/// if the decoder is not generating frames
		if (!m_Impl->m_IsGenerating.load(std::memory_order_seq_cst))
		{
			auto curFrameId = m_Impl->m_DecodingNode->GetCurrentFramePosition();
			auto duration = m_Impl->m_DecodingNode->GetFrameCount();
			if (curFrameId < duration)
			{
				/// set state to playing
				m_Impl->m_IsPlaying.store(true, std::memory_order_seq_cst);
				/// generate a frame
				m_Impl->m_IsGenerating.store(true, std::memory_order_seq_cst);
				m_Impl->m_DecodingNode->SetCurrentFramePos(curFrameId + 1);
				m_Impl->m_DecodingNode->GenerateFrame();
				m_Impl->m_IsGenerating.store(false, std::memory_order_seq_cst);
				m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(curFrameId + 1);
				if (!m_Impl->m_FrameQueue.empty())
				{
					m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameQueue.front());
					m_Impl->m_FrameQueue.pop_front();
				}
			}
			else
			{
				Stop();
			}
		}
	}));
}
///	\brief pause playback
void PlayerModel::Pause()
{
	while (m_Impl->m_IsGenerating.load(std::memory_order_seq_cst))
	{

	}
	m_Impl->m_PlaybackLifetime.clear();
	m_Impl->m_IsGenerating.store(false, std::memory_order_seq_cst);
	m_Impl->m_IsPlaying.store(false, std::memory_order_seq_cst);
}
///	\brief stio playback
void PlayerModel::Stop()
{
	while (m_Impl->m_IsGenerating.load(std::memory_order_seq_cst))
	{

	}
	m_Impl->m_PlaybackLifetime.clear();
	m_Impl->m_IsGenerating.store(true, std::memory_order_seq_cst);
	m_Impl->m_DecodingNode->SetCurrentFramePos(0);
	m_Impl->m_DecodingNode->GenerateFrame();
	m_Impl->m_IsGenerating.store(false, std::memory_order_seq_cst);
	m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(0);
	m_Impl->m_IsPlaying.store(false, std::memory_order_seq_cst);
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

rxcpp::observable<PlayerModel::frame_t> fu::fusion::PlayerModel::CurrentFrameFlowOut()
{
	//return m_Impl->m_FrameFlowOutSubj.get_observable().as_dynamic();
	return m_Impl->m_DecodingNode->FrameFlowOut();
}

rxcpp::observable<int> fu::fusion::PlayerModel::FrameWidthFlowOut()
{
	return m_Impl->m_FrameWidthFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<int> fu::fusion::PlayerModel::FrameHeightFlowOut()
{
	return m_Impl->m_FrameHeightFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<uint2> fu::fusion::PlayerModel::FrameSizeFlowOut()
{
	return m_Impl->m_FrameSizeFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu