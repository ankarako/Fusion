#include <Models/PlayerModel.h>
#include <Core/Coordination.h>
#include <DecodingNode.h>
#include <TextureExportingNode.h>
#include <Core/SettingsRepo.h>
#include <Core/ThreadSafeQueue.h>
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
	static constexpr size_t	k_InitialFramesPrefetch = 128;
	static constexpr size_t k_FrameQueueSizeThreshold = k_InitialFramesPrefetch / 2;
	static constexpr float k_FrameQueueSizeFactor = 1.5f;
	///	\typedef settings_ptr_t
	///	\brief type of a pointer to our settings type
	using settings_ptr_t = std::shared_ptr<PlayerSettings>;
	/// coordination
	coord_ptr_t						m_Coord;
	///	a decoding node used to decode video files
	trans::DecodingNode				m_DecodingNode{ nullptr };
	trans::TextureExportingNode		m_EncodingNode{ nullptr };
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
	std::atomic_bool				m_SignalledGeneration{ false };
	bool							m_IsGenerating{ false };
	/// Our settingss
	srepo_ptr_t						m_SettingsRepo;
	settings_ptr_t					m_Settings;
	/// framme FIFO queue	
	using frame_data_t = std::pair<int, frame_t>;
	std::map<int, frame_t>					m_FrameMap;
	ThreadSafeQueue<frame_data_t>			m_FrameQueue;
	///	frame rate (actually frame period)
	std::chrono::milliseconds			m_FramePeriod;
	/// current frame id output
	rxcpp::subjects::subject<size_t>	m_CurrentFrameIdFlowOutSubj;
	/// current frame count
	rxcpp::subjects::subject<size_t>	m_FrameCountFlowOutSubj;
	///	frame width output
	rxcpp::subjects::subject<size_t>	m_FrameWidthFlowOutSubj;
	///	frame height output
	rxcpp::subjects::subject<size_t>	m_FrameHeightFlowOutSubj;
	/// frame size output
	rxcpp::subjects::subject<uint2>		m_FrameSizeFlowOutSubj;
	///	current frame output
	rxcpp::subjects::subject<frame_t>	m_FrameFlowOutSubj;
	///	start prefetch event
	rxcpp::subjects::subject<void*>		m_StartPrefetchEventSubj;

	rxcpp::subjects::subject<void*>		m_OnVideoLoadedSubj;

	rxcpp::subjects::subject<void*>		m_ChromaKeyBGFlowInSubj;

	rxcpp::subjects::subject<void*>		m_RemoveChromaKeyBGFlowInSubj;

	rxcpp::subjects::subject<SequenceItem>	m_SequenceItemFlowOutSubj;

	rxcpp::subjects::subject<float>			m_FrameLoadProgressFlowOutSubj;
	rxcpp::subjects::subject<std::string>	m_ProgressMsgFlowOutSubj;
	/// Construction
	/// \brief default constructor
	///	does nothing
	Impl(coord_ptr_t coord, srepo_ptr_t srepo)
		: m_Coord(coord), m_SettingsRepo(srepo), m_DecodingNode(trans::CreateDecodingNode()), m_EncodingNode(trans::CreateTextureExportingNode())
	{ 
		m_IsPlaying.store(false, std::memory_order_seq_cst);
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
			//this->Seek(m_Impl->m_Settings->CurrentFrameId);
			//BufferCPU<uchar4> firstFrame = m_Impl->m_DecodingNode->GetFrame(0);
			//m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(firstFrame);
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
	/// operates on new thread
	///=========================
	m_Impl->m_StartPrefetchEventSubj.get_observable().as_dynamic()
		.observe_on(rxcpp::observe_on_new_thread())
		.subscribe([this](auto _) 
	{
		m_Impl->m_IsGenerating = true;
		m_Impl->m_DecodingNode->GenerateFrames(m_Impl->k_InitialFramesPrefetch);
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
				LOG_ERROR << "Error when prefetching frames: " << ex.what();
			}
		}
	});
	///================================================
	///	Prefetching frames completed notification task:
	///	set is generating to false
	///================================================
	m_Impl->m_DecodingNode->PrefetchFramesCompleted()
		//.observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](auto _)
	{
		m_Impl->m_IsGenerating = false;
	});
	///==============================
	/// Decoding node output to queue
	///==============================
	m_Impl->m_DecodingNode->FrameFlowOut()
		.subscribe([this](const std::pair<int, BufferCPU<uchar4>>& frameData) 
	{
		m_Impl->m_FrameMap.insert(frameData);	
	});

	m_Impl->m_ChromaKeyBGFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](auto _) 
	{
		m_Impl->m_DecodingNode->SetChromaKeyBGEnabled(true);
		m_Impl->m_DecodingNode->SetChromaKeyBGVal(make_uchar4(0, 255, 0, 255));
		this->Stop();
		//m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
	});

	m_Impl->m_RemoveChromaKeyBGFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](auto _) 
	{
		m_Impl->m_DecodingNode->SetChromaKeyBGEnabled(false);
		this->Stop();
	});
}
///	\brief destroy the player
///	Closes the MFSession
void PlayerModel::Destroy()
{
	if (m_Impl->m_DecodingNode->IsGenerating())
	{
		m_Impl->m_DecodingNode->SetGeneratingFrames(false);
	}
	/// if decoder is generating frames wait
	while (m_Impl->m_DecodingNode->IsGenerating())
	{

	}
	/// Stop (clears frame queue)
	Stop();
	/// Release the decoding node
	m_Impl->m_DecodingNode->Release();
}
///	\brief load a media file
///	\param	filepath	the path of the file to load
void PlayerModel::LoadFile(const std::string& filepath)
{
	/// check if there is alreaduy a file loaded
	if (!m_Impl->m_Settings->LoadedVideoFilepath.empty() || m_Impl->m_DecodingNode->IsGenerating() || m_Impl->m_IsPlaying.load(std::memory_order_seq_cst))
	{
		//Destroy();
	}
	std::string filename = std::filesystem::path(filepath).filename().generic_string();
	std::string msg = "Loading: " + filename;
	m_Impl->m_ProgressMsgFlowOutSubj.get_subscriber().on_next(msg);
	m_Impl->m_FrameLoadProgressFlowOutSubj.get_subscriber().on_next(0.0f);
	/// load in the decoder
	m_Impl->m_DecodingNode->LoadFile(filepath);
	m_Impl->m_Settings->LoadedVideoFilepath = filepath;
	/// create our scaling node
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
	m_Impl->m_PlaybackObs = rxcpp::observable<>::interval(periodNano).as_dynamic();
	
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
	/// attach playback callbacks
	///===============
	/// Start Playback
	///===============
	item.OnStartPlayback.get_observable().as_dynamic()
		.observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](auto _) 
	{
	
	
	});
	///================
	/// Pause playback
	///================
	item.OnPause.get_observable().as_dynamic()
		.observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](auto _) 
	{
		this->Pause();
	});
	///===============
	/// Stop playback
	///===============
	item.OnStop.get_observable().as_dynamic()
		.observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](auto _) 
	{
		this->Stop();
	});
	///=======
	/// Seek
	///=======
	item.OnSeekFrame.get_observable().as_dynamic()
		.observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](int frameId) 
	{
		this->Seek(frameId);
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
	///==============
	/// Seek Backward
	///==============
	item.OnSeekBackward.get_observable().as_dynamic()
		.observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](auto _) 
	{
		this->SeekBackward();
	});
	///==============
	/// Seek Forward
	///==============
	item.OnSeekForward.get_observable().as_dynamic()
		.observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](auto _)
	{
		this->SeekForward();
	});
	m_Impl->m_SequenceItemFlowOutSubj.get_subscriber().on_next(item);
	///===================
	/// start prefetching
	///===================
	m_Impl->m_OnVideoLoadedSubj.get_subscriber().on_next(nullptr);
	/// prefetch frames
	m_Impl->m_DecodingNode->SetCurrentFramePos(0);
	m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_DecodingNode->GetFrame(0));
	//// needed beacause now the decoding node is at frame 1
	m_Impl->m_DecodingNode->SetCurrentFramePos(0);
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
		m_Impl->m_PlaybackObs.subscribe_on(m_Impl->m_Coord->ModelCoordination()).subscribe(
			[this](auto _)
	{
		Seek(m_Impl->m_Settings->CurrentFrameId);
		m_Impl->m_Settings->CurrentFrameId++;
	}));
}
///	\brief pause playback
void PlayerModel::Pause()
{
	
}
///	\brief stop playback
void PlayerModel::Stop()
{	
	m_Impl->m_IsGenerating = false;
	auto it = m_Impl->m_FrameMap.find(0);
	if (it != m_Impl->m_FrameMap.end())
	{
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(it->second);
		m_Impl->m_Settings->CurrentFrameId = it->first;
		m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(m_Impl->m_Settings->CurrentFrameId);
		m_Impl->m_FrameMap.clear();
		m_Impl->m_DecodingNode->SetCurrentFramePos(0);
		m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
	}
	else
	{
		while (m_Impl->m_DecodingNode->IsGenerating())
		{
			LOG_WARNING << "Generating...";
		}
		m_Impl->m_FrameMap.clear();
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_DecodingNode->GetFrame(0));
		if (!m_Impl->m_IsGenerating)
		{
			m_Impl->m_DecodingNode->SetCurrentFramePos(0);
			m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
		}
	}
}
///	\brief seek to a specified frame position
void PlayerModel::Seek(int framePos)
{
	if (!m_Impl->m_FrameMap.empty())
	{
		auto it = m_Impl->m_FrameMap.find(framePos);
		int frameCount = m_Impl->m_DecodingNode->GetFrameCount();
		int lastFrameId = m_Impl->m_FrameMap.rbegin()->first;
		if (it != m_Impl->m_FrameMap.end())
		{
			m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(it->second);
			m_Impl->m_Settings->CurrentFrameId = it->first;
			m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(m_Impl->m_Settings->CurrentFrameId);
			m_Impl->m_FrameMap.erase(framePos);
		}
		bool frameMapSize = m_Impl->m_FrameMap.size() < (m_Impl->k_FrameQueueSizeThreshold * m_Impl->k_FrameQueueSizeFactor);
		bool isGenerating = !m_Impl->m_IsGenerating;
		bool lastFrame = lastFrameId != frameCount - 1;
		if (frameMapSize && isGenerating && lastFrame)
		{
			m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
		}
	}
	else
	{
		m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
	}
}
///	\brief seek forward one frame
void PlayerModel::SeekForward()
{
	int frame = m_Impl->m_Settings->CurrentFrameId + 1;
	auto it = m_Impl->m_FrameMap.find(frame);
	if (it != m_Impl->m_FrameMap.end())
	{
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(it->second);
		m_Impl->m_Settings->CurrentFrameId = frame;
		m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(m_Impl->m_Settings->CurrentFrameId);
	}
	else
	{	
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_DecodingNode->GetFrame(frame));
		m_Impl->m_Settings->CurrentFrameId = frame;
		m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(m_Impl->m_Settings->CurrentFrameId);
		m_Impl->m_FrameMap.clear();
		m_Impl->m_DecodingNode->SetCurrentFramePos(m_Impl->m_Settings->CurrentFrameId);
		m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
	}
}
///	\brief seek backward one frame
void PlayerModel::SeekBackward()
{
	int frame = m_Impl->m_Settings->CurrentFrameId - 1;
	auto it = m_Impl->m_FrameMap.find(frame);
	if (it != m_Impl->m_FrameMap.end())
	{
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(it->second);
		m_Impl->m_Settings->CurrentFrameId = frame;
		m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(m_Impl->m_Settings->CurrentFrameId);
	}
	else
	{
		while (m_Impl->m_DecodingNode->IsGenerating())
		{

		}
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_DecodingNode->GetFrame(frame));
		m_Impl->m_Settings->CurrentFrameId = frame;
		m_Impl->m_CurrentFrameIdFlowOutSubj.get_subscriber().on_next(m_Impl->m_Settings->CurrentFrameId);
		m_Impl->m_FrameMap.clear();
		//m_Impl->m_DecodingNode->SetCurrentFramePos(m_Impl->m_Settings->CurrentFrameId);
		//m_Impl->m_StartPrefetchEventSubj.get_subscriber().on_next(nullptr);
	}
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
	auto it = m_Impl->m_FrameMap.find(m_Impl->m_Settings->CurrentFrameId);
	if (it != m_Impl->m_FrameMap.end())
	{
		return it->second;
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

rxcpp::observable<size_t> fu::fusion::PlayerModel::FrameWidthFlowOut()
{
	return m_Impl->m_FrameWidthFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<size_t> fu::fusion::PlayerModel::FrameHeightFlowOut()
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
rxcpp::observer<void*> PlayerModel::ChromaKeyBGFlowIn()
{
	return m_Impl->m_ChromaKeyBGFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<void*> PlayerModel::RemoveChromaKeyBGFlowIn()
{
	return m_Impl->m_RemoveChromaKeyBGFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observable<std::string> PlayerModel::ProgressMessageFlowOut()
{
	return m_Impl->m_ProgressMsgFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<float> PlayerModel::LoadingProgressFlowOut()
{
	return m_Impl->m_FrameLoadProgressFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu