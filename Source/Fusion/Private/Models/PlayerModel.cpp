#include <Models/PlayerModel.h>
#include <Core/Coordination.h>
#include <DecodingNode.h>
#include <ScalingNode.h>
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
	///	const initial buffer prefetching samples
	static constexpr size_t	k_InitialFramesPrefetch = 120;
	///	\typedef settings_ptr_t
	///	\brief type of a pointer to our settings type
	using settings_ptr_t = std::shared_ptr<PlayerSettings>;
	/// coordination
	coord_ptr_t						m_Coord;
	///	a decoding node used to decode video files
	trans::DecodingNode				m_DecodingNode{ nullptr };
	/// a scaling node for scaling the incoming decoded frames
	trans::ScalingNode				m_ScalingNode{ nullptr };
	///
	rxcpp::observable<long>			m_PlaybackObs;
	rxcpp::observable<long>			m_CheckPrefetchObs;
	/// playback subscription
	rxcpp::composite_subscription	m_PlaybackLifetime;
	rxcpp::composite_subscription	m_NodeTopology;
	rxcpp::composite_subscription	m_PrefetchLifetime;
	///	atomic flags for signifying playbck and frame generation
	std::atomic_bool				m_IsPlaying{ false };
	std::atomic_bool				m_IsGenerating{ false };
	std::atomic_bool				m_SignalledGeneration{ false };
	/// Our settingss
	srepo_ptr_t						m_SettingsRepo;
	settings_ptr_t					m_Settings;
	/// framme FIFO queue	
	std::deque<frame_t>				m_FrameQueue;
	///	frame rate (actually frame period)
	std::chrono::milliseconds			m_FramePeriod;
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
	///	start prefetch event
	rxcpp::subjects::subject<void*>		m_StartPrefetchEventSubj;
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
	///================================================
	///	Prefetching frames completed notification task:
	///	set is generating to false
	///================================================
	m_Impl->m_DecodingNode->PrefetchFramesCompleted().observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](auto _) 
	{
		m_Impl->m_IsGenerating.store(false, std::memory_order_seq_cst);
	});

	/// create out settings
	m_Impl->m_Settings = std::make_shared<PlayerSettings>();
	/// register them to the repo
	m_Impl->m_SettingsRepo->RegisterSettings(m_Impl->m_Settings);
	/// set settinsgs frames prtefetch
	m_Impl->m_Settings->PrefetchFrameCount = m_Impl->k_InitialFramesPrefetch;
}
///	\brief destroy the player
///	Closes the MFSession
void PlayerModel::Destroy()
{
	/// if decoder is generating frames wait
	while (m_Impl->m_IsGenerating.load(std::memory_order_seq_cst))
	{

	}
	Stop();
	/// clear the frame queue
	m_Impl->m_FrameQueue.clear();
	m_Impl->m_DecodingNode->Release();
}
///	\brief load a media file
///	\param	filepath	the path of the file to load
void PlayerModel::LoadFile(const std::string& filepath)
{
	/// load 
	m_Impl->m_DecodingNode->LoadFile(filepath);
	m_Impl->m_Settings->LoadedVideoFilepath = filepath;
	/// create our scaling node
	m_Impl->m_ScalingNode = trans::CreateScalingNode(m_Impl->m_DecodingNode->GetFrameWidth(), m_Impl->m_DecodingNode->GetFrameHeight());
	/// check fps to create interval observable
	size_t frameCount = m_Impl->m_DecodingNode->GetFrameCount();
	double fps = m_Impl->m_DecodingNode->GetFrameRate();
	double periodSecs = 1.0 / fps;
	m_Impl->m_FramePeriod = 
		std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(periodSecs));
	/// now that we know everything about the file
	///	we should create a prefetching technique so that we can play large files
	m_Impl->m_FrameCountFlowOutSubj.get_subscriber().on_next(frameCount);
	m_Impl->m_FrameWidthFlowOutSubj.get_subscriber().on_next(m_Impl->m_DecodingNode->GetFrameWidth());
	m_Impl->m_FrameHeightFlowOutSubj.get_subscriber().on_next(m_Impl->m_DecodingNode->GetFrameHeight());
	m_Impl->m_FrameSizeFlowOutSubj.get_subscriber().on_next(make_uint2(m_Impl->m_DecodingNode->GetFrameWidth(), m_Impl->m_DecodingNode->GetFrameHeight()));
	/// create the interval observable according to the frame rate
	std::chrono::nanoseconds periodNano = std::chrono::duration_cast<std::chrono::nanoseconds>(m_Impl->m_FramePeriod);
	m_Impl->m_PlaybackObs		= rxcpp::observable<>::interval(periodNano).as_dynamic();
	m_Impl->m_CheckPrefetchObs	
		= rxcpp::observable<>::interval(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(1.0f))).as_dynamic();
	
	m_Impl->m_NodeTopology.add(
	///====================================
	/// hook scaling node to decodning node
	///====================================
	m_Impl->m_DecodingNode->NativeFrameFlowOut()
		.subscribe(m_Impl->m_ScalingNode->NativeFrameFlowIn()));

	m_Impl->m_NodeTopology.add(
	///=================================
	///	scaling node Frame flow in Task
	///	push frame in buffer
	///=================================
	m_Impl->m_DecodingNode->FrameFlowOut()
		.subscribe([this](frame_t frame)
	{
		m_Impl->m_FrameQueue.emplace_back(frame);
	}));
	/// start prefetching
	m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
}
///	\brief start playback
void PlayerModel::Start()
{
	LOG_DEBUG << "Started PLayback on thread: " << std::this_thread::get_id();
	m_Impl->m_IsPlaying.store(true, std::memory_order_seq_cst);
	/// Play one frame task
	m_Impl->m_PlaybackLifetime.add(
		m_Impl->m_PlaybackObs.subscribe(
			[this](auto _)
	{
		size_t dur	= m_Impl->m_DecodingNode->GetFrameCount();
		size_t dist = m_Impl->m_Settings->CurrentFrameId;
		size_t queuesz = m_Impl->m_FrameQueue.size();

		if (!m_Impl->m_FrameQueue.empty() && m_Impl->m_IsPlaying.load(std::memory_order_seq_cst))
		{
			m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameQueue.front());
			m_Impl->m_Settings->CurrentFrameId++;
			m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(m_Impl->m_Settings->CurrentFrameId);
			m_Impl->m_FrameQueue.pop_front();
		}
	}));

	///=================================================
	/// tasks that checks the queue size for prefetching
	///=================================================
	m_Impl->m_PlaybackLifetime.add(
		m_Impl->m_CheckPrefetchObs.subscribe(
			[this](auto _)
	{
		m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
	}));
	///============================
	///	initialize prefetching task
	///=============================
	m_Impl->m_PlaybackLifetime.add(
		m_Impl->m_StartPrefetchEventSubj.get_observable().observe_on(rxcpp::synchronize_new_thread())
		.subscribe([this](auto _)
	{
		m_Impl->m_IsGenerating.store(true, std::memory_order_seq_cst);
		m_Impl->m_DecodingNode->GenerateFrames((unsigned int)ceilf(m_Impl->m_DecodingNode->GetFrameRate()));
	}));
}
///	\brief pause playback
void PlayerModel::Pause()
{
	m_Impl->m_PlaybackLifetime.clear();
	while (m_Impl->m_IsGenerating.load(std::memory_order_seq_cst))
	{

	}
	m_Impl->m_IsPlaying.store(false, std::memory_order_seq_cst);
}
///	\brief stop playback
void PlayerModel::Stop()
{	
	if (this->IsOpen())
	{
		Pause();
		/// set playback
		/// clear playback subscription
		//m_Impl->m_PrefetchLifetime.clear();
		/*m_Impl->m_PlaybackLifetime.clear();
		m_Impl->m_IsPlaying.store(false, std::memory_order_seq_cst);*/
		while (m_Impl->m_IsGenerating.load(std::memory_order_seq_cst))
		{
			LOG_DEBUG << "Stop. Waiting";
		}
		
		/// erase frame queue
		m_Impl->m_FrameQueue.clear();
		/// generate the first frame of the video
		m_Impl->m_IsGenerating.store(true, std::memory_order_seq_cst);
		m_Impl->m_DecodingNode->SetCurrentFramePos(0);
		m_Impl->m_DecodingNode->GenerateFrame();
		m_Impl->m_IsGenerating.store(false, std::memory_order_seq_cst);
		/// send the first frame
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameQueue.back());
		/// set current frame
		m_Impl->m_Settings->CurrentFrameId = 0;
		m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(0);
		/// signal a prefetch event
		m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
		/// and we should be OK
	}
}
///	\brief seek to a specified frame position
void PlayerModel::Seek(int framePos)
{
	bool wasPlaying = m_Impl->m_IsPlaying.load(std::memory_order_seq_cst);
	Pause();
	m_Impl->m_FrameQueue.clear();
	m_Impl->m_IsGenerating.store(true, std::memory_order_seq_cst);
	m_Impl->m_DecodingNode->SetCurrentFramePos(framePos);
	m_Impl->m_Settings->CurrentFrameId = framePos;
	m_Impl->m_DecodingNode->GenerateFrame();
	m_Impl->m_IsGenerating.store(false, std::memory_order_seq_cst);
	/// send the frame
	m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameQueue.back());
	m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(m_Impl->m_Settings->CurrentFrameId);
	if (wasPlaying)
		Start();
}
///	\brief seek forward one frame
void PlayerModel::SeekForward()
{
	while (m_Impl->m_IsGenerating.load(std::memory_order_seq_cst))
	{
		/// do nothing
	}
	Pause();
	m_Impl->m_IsGenerating.store(true, std::memory_order_seq_cst);
	m_Impl->m_Settings->CurrentFrameId++;
	m_Impl->m_DecodingNode->SetCurrentFramePos(m_Impl->m_Settings->CurrentFrameId);
	m_Impl->m_DecodingNode->GenerateFrame();
	m_Impl->m_IsGenerating.store(false, std::memory_order_seq_cst);
	/// send the frame
	m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameQueue.back());
	m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(m_Impl->m_Settings->CurrentFrameId);
}
///	\brief seek backward one frame
void PlayerModel::SeekBackward()
{
	while (m_Impl->m_IsGenerating.load(std::memory_order_seq_cst))
	{
		/// do nothing
	}
	Pause();
	m_Impl->m_IsGenerating.store(true, std::memory_order_seq_cst);
	m_Impl->m_Settings->CurrentFrameId--;
	m_Impl->m_DecodingNode->SetCurrentFramePos(m_Impl->m_Settings->CurrentFrameId);
	m_Impl->m_DecodingNode->GenerateFrame();
	m_Impl->m_IsGenerating.store(false, std::memory_order_seq_cst);
	/// send the frame
	m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameQueue.back());
	m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(m_Impl->m_Settings->CurrentFrameId);
}
///	\brief set the number of frames to prefetch
///	\param	frameCount	the number of frames to prefetch
void fu::fusion::PlayerModel::SetFramePrefetchCount(size_t frameCount)
{
	m_Impl->m_Settings->PrefetchFrameCount = frameCount;
}
/// \brief checj if the model has opened a file
///	\return true if the video player is opened
bool fu::fusion::PlayerModel::IsOpen() const
{
	return !m_Impl->m_Settings->LoadedVideoFilepath.empty();
}
/// \brief set the scaling size of the video
///	\param	size	the size to set
void fu::fusion::PlayerModel::SetScalingSize(uint2 size)
{
	if (!m_Impl->m_Settings->LoadedVideoFilepath.empty())
	{
		while (m_Impl->m_IsGenerating.load(std::memory_order_seq_cst))
		{

		}
		m_Impl->m_NodeTopology.clear();
		//m_Impl->m_ScalingNode = trans::CreateScalingNode(s.x, s.y);
		m_Impl->m_ScalingNode->SetScalingSize(size);
		m_Impl->m_NodeTopology.add(
			/// hook scaling node to decodning node
			m_Impl->m_DecodingNode->NativeFrameFlowOut()
			.subscribe(m_Impl->m_ScalingNode->NativeFrameFlowIn()));
		m_Impl->m_NodeTopology.add(
			///=================================
			///	scaling node Frame flow in Task
			///	push frame in buffer
			///=================================
			m_Impl->m_ScalingNode->FrameFlowOut()
			.subscribe([this](frame_t frame)
		{
			m_Impl->m_FrameQueue.emplace_back(frame);
		}));
		/// start prefetching
		m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
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

rxcpp::observable<PlayerModel::frame_t> fu::fusion::PlayerModel::CurrentFrameFlowOut()
{
	return m_Impl->m_FrameFlowOutSubj.get_observable().as_dynamic();
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