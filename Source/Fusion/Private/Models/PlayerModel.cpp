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
#include <filesystem>

namespace fu {
namespace fusion {
///	\brief Video player model implementation
struct PlayerModel::Impl
{
	///	const initial buffer prefetching samples
	static constexpr size_t	k_InitialFramesPrefetch = 256;
	static constexpr size_t k_FrameQueueSizeThreshold = k_InitialFramesPrefetch / 2;
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
	std::chrono::steady_clock::time_point	m_StartedPlayback;
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
	using frame_data_t = std::pair<int, frame_t>;
	std::deque<frame_data_t>			m_FrameQueue;
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

	rxcpp::subjects::subject<void*>		m_OnVideoLoadedSubj;

	rxcpp::subjects::subject<SequenceItem> m_SequenceItemFlowOutSubj;
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
	/// create out settings
	m_Impl->m_Settings = std::make_shared<PlayerSettings>();
	m_Impl->m_SettingsRepo->RegisterSettings(m_Impl->m_Settings);
	m_Impl->m_Settings->PrefetchFrameCount = m_Impl->k_InitialFramesPrefetch;
	/// settings loaded task
	/// FIXME: take(1) is a workaround because subscription hits two times for some reason
	m_Impl->m_Settings->OnSettingsLoaded().take(1)
		.subscribe([this](auto _)
	{
		if (!m_Impl->m_Settings->LoadedVideoFilepath.empty())
		{
			this->LoadFile(m_Impl->m_Settings->LoadedVideoFilepath);
			this->Seek(m_Impl->m_Settings->CurrentFrameId);
		}
	}, [this](std::exception_ptr ptr) 
	{
		if (ptr)
		{
			try
			{
				std::rethrow_exception(ptr);
			}
			catch (std::exception & ex)
			{
				LOG_ERROR << ex.what();
			}
		}
	});
	///=========================
	/// Prefetching frames task
	///=========================
	m_Impl->m_StartPrefetchEventSubj.get_observable().as_dynamic()
		.observe_on(rxcpp::observe_on_new_thread())
		.subscribe([this](auto _) 
	{
		m_Impl->m_DecodingNode->SetGeneratingFrames(true);
		m_Impl->m_IsGenerating.store(true, std::memory_order_seq_cst);
		m_Impl->m_DecodingNode->GenerateFrames(m_Impl->k_InitialFramesPrefetch);
	});
	///================================================
	///	Prefetching frames completed notification task:
	///	set is generating to false
	///================================================
	m_Impl->m_DecodingNode->PrefetchFramesCompleted().observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](auto _)
	{
		m_Impl->m_IsGenerating.store(false, std::memory_order_seq_cst);
		m_Impl->m_DecodingNode->SetGeneratingFrames(false);
	});
	///==============================
	/// Decoding node output to queue
	///==============================
	m_Impl->m_DecodingNode->FrameFlowOut()
		.subscribe([this](const BufferCPU<uchar4>& frameData) 
	{
		auto data = std::make_pair(m_Impl->m_DecodingNode->GetCurrentFramePosition(), frameData);
		m_Impl->m_FrameQueue.emplace_back(data);
	});
}
///	\brief destroy the player
///	Closes the MFSession
void PlayerModel::Destroy()
{
	if (m_Impl->m_IsGenerating.load(std::memory_order_seq_cst))
	{
		m_Impl->m_DecodingNode->SetGeneratingFrames(false);
	}
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
	std::string filename = std::filesystem::path(filepath).filename().generic_string();
	/// load 
	m_Impl->m_DecodingNode->LoadFile(filepath);
	m_Impl->m_Settings->LoadedVideoFilepath = filepath;
	/// create our scaling node
	//m_Impl->m_ScalingNode = trans::CreateScalingNode(m_Impl->m_DecodingNode->GetFrameWidth(), m_Impl->m_DecodingNode->GetFrameHeight());
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
	
	SequenceItem item; 
	item.Name = filename;
	item.Type = SequenceItemType::Video;
	item.FrameStart = 0;
	item.Duration = m_Impl->m_DecodingNode->GetFrameCount();
	item.FrameEnd = item.Duration;
	item.Expanded = false;
	item.SeqFrameStart = 0;
	item.SeqFrameEnd = item.Duration;
	item.Id = 1;
	m_Impl->m_SequenceItemFlowOutSubj.get_subscriber().on_next(item);
	///===================
	/// start prefetching
	///===================
	m_Impl->m_OnVideoLoadedSubj.get_subscriber().on_next(nullptr);
	m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
}
///==================
/// Playback function
///==================
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
		bool playbackPred = m_Impl->m_FrameQueue.size() > m_Impl->k_FrameQueueSizeThreshold && m_Impl->m_IsPlaying.load(std::memory_order_seq_cst);
		if (playbackPred)
		{
			m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameQueue.front());
			m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(m_Impl->m_Settings->CurrentFrameId);
			m_Impl->m_Settings->CurrentFrameId++;
			m_Impl->m_FrameQueue.pop_front();
		}
		if (m_Impl->m_FrameQueue.size() <= m_Impl->k_FrameQueueSizeThreshold && !m_Impl->m_DecodingNode->IsGenerating())
		{
			m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
		}
	}));
}
///	\brief pause playback
void PlayerModel::Pause()
{
	m_Impl->m_DecodingNode->SetGeneratingFrames(false);
	m_Impl->m_PlaybackLifetime.clear();
	m_Impl->m_IsPlaying.store(false, std::memory_order_seq_cst);
	while (m_Impl->m_IsGenerating.load(std::memory_order_seq_cst))
	{
		//LOG_DEBUG << "Generating";
	}
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
		//while (m_Impl->m_IsGenerating.load(std::memory_order_seq_cst))
		//{
		//	//LOG_DEBUG << "Stop. Waiting";
		//}
		
		/// erase frame queue
		m_Impl->m_FrameQueue.clear();
		/// generate the first frame of the video
		m_Impl->m_IsGenerating.store(true, std::memory_order_seq_cst);
		m_Impl->m_DecodingNode->SetCurrentFramePos(0);
		m_Impl->m_DecodingNode->GenerateFrame();
		m_Impl->m_IsGenerating.store(false, std::memory_order_seq_cst);
		/// send the first frame
		
		/// set current frame
		m_Impl->m_Settings->CurrentFrameId = 0;
		m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(0);
		/// signal a prefetch event
		if (!m_Impl->m_FrameQueue.empty())
			m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameQueue.back());
		m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
		/// and we should be OK
	}
}
///	\brief seek to a specified frame position
void PlayerModel::Seek(int framePos)
{
	//bool wasPlaying = m_Impl->m_IsPlaying.load(std::memory_order_seq_cst);
	//Pause();
	//m_Impl->m_FrameQueue.clear();
	//m_Impl->m_IsGenerating.store(true, std::memory_order_seq_cst);
	//m_Impl->m_DecodingNode->SetCurrentFramePos(framePos);
	//m_Impl->m_Settings->CurrentFrameId = framePos;
	//m_Impl->m_DecodingNode->GenerateFrame();
	//m_Impl->m_IsGenerating.store(false, std::memory_order_seq_cst);
	///// send the frame
	//m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_FrameQueue.back());
	//m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(m_Impl->m_Settings->CurrentFrameId);
	//m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
	if (!m_Impl->m_FrameQueue.empty())
	{
		if (std::find(m_Impl->m_FrameQueue.begin(), m_Impl->m_FrameQueue.end(), [this, framePos](auto _) { return framePos == _.first}))
		{

		}
	}
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
	//if (!m_Impl->m_Settings->LoadedVideoFilepath.empty())
	//{
	//	while (m_Impl->m_IsGenerating.load(std::memory_order_seq_cst))
	//	{

	//	}
	//	m_Impl->m_NodeTopology.clear();
	//	//m_Impl->m_ScalingNode = trans::CreateScalingNode(s.x, s.y);
	//	m_Impl->m_ScalingNode->SetScalingSize(size);
	//	m_Impl->m_NodeTopology.add(
	//		/// hook scaling node to decodning node
	//		m_Impl->m_DecodingNode->NativeFrameFlowOut()
	//		.subscribe(m_Impl->m_ScalingNode->NativeFrameFlowIn()));
	//	m_Impl->m_NodeTopology.add(
	//		///=================================
	//		///	scaling node Frame flow in Task
	//		///	push frame in buffer
	//		///=================================
	//		m_Impl->m_ScalingNode->FrameFlowOut()
	//		.subscribe([this](frame_t frame)
	//	{
	//		m_Impl->m_FrameQueue.emplace_back(frame);
	//	}));
	//	/// start prefetching
	//	m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
	//}
}

uint2 fu::fusion::PlayerModel::GetFrameSize() const
{
	return make_uint2(m_Impl->m_DecodingNode->GetFrameWidth(), m_Impl->m_DecodingNode->GetFrameHeight());
}

const BufferCPU<uchar4>& fu::fusion::PlayerModel::GetCurrentFrame()
{
	if (!m_Impl->m_FrameQueue.empty())
	{
		return m_Impl->m_FrameQueue.front().second;
	}
	return CreateBufferCPU<uchar4>(0);
}
int PlayerModel::GetCurrentFrameId() const
{
	return m_Impl->m_Settings->CurrentFrameId;
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

rxcpp::observable<void*> fu::fusion::PlayerModel::OnVideoLoaded()
{
	return	m_Impl->m_OnVideoLoadedSubj.get_observable().as_dynamic();
}

rxcpp::observable<SequenceItem> fu::fusion::PlayerModel::SequenceItemFlowOut()
{
	return m_Impl->m_SequenceItemFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu