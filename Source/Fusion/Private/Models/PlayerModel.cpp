#include <Models/PlayerModel.h>
#include <Transcoder.h>

namespace fusion {
///	\struct Impl
///	\brief	PlayerModel implementation
struct PlayerModel::Impl
{
	///	\typedef transcoder_ptr_t
	using transcoder_ptr_t = std::shared_ptr<video::Transcoder>;
	/// video transcoder instance
	transcoder_ptr_t	m_Transcoder;
	///	current frame output
	rxcpp::subjects::subject<vframe_buffer_t> m_CurrentVideoFrameFlowOutSubj;
	///	current frame output
	rxcpp::subjects::subject<aframe_buffer_t> m_CurrentAudioFrameFlowOutSubj;
	/// Construction
	Impl() 
		: m_Transcoder(std::make_shared<video::Transcoder>())
	{ }
};	///	!struct Impl
/// Construction
///	\brief default constructor
PlayerModel::PlayerModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
///	Destruction
PlayerModel::~PlayerModel() 
{ 
	Destroy(); 
}
///	\brief Initialization
void PlayerModel::Init()
{
	m_Impl->m_Transcoder->InitializeContext();
}

void PlayerModel::Destroy()
{
	m_Impl->m_Transcoder->Destroy();
}
///	\brief open a video file
///	\param	filepath	the file to opoen
void PlayerModel::LoadFile(const std::string& filepath)
{
	m_Impl->m_Transcoder->LoadFile(filepath);
	m_Impl->m_Transcoder->InitializeDecoderContext();
	m_Impl->m_Transcoder->InitializeSwScaleContext();
}
///	\brief get the loaded video's frame count
///	\return the video's frame count
size_t PlayerModel::GetFrameCount() const
{
	return 0;
}
///	\brief get the current frame in the loaded frame
///	\return	the current frame index
size_t PlayerModel::GetCurrentFrameIndex() const
{
	return 0;
}
///	\brief go to specific frame in the video
///	\param	index	the frame index to seek
void PlayerModel::SeekFrame(size_t index)
{

}
///	\brief start playback
void PlayerModel::Play()
{
	m_Impl->m_Transcoder->DecoderStep();
	vframe_buffer_t vframe = m_Impl->m_Transcoder->GetCurrentVideoFrame();
	m_Impl->m_CurrentVideoFrameFlowOutSubj.get_subscriber().on_next(vframe);
	aframe_buffer_t aframe = m_Impl->m_Transcoder->GetCurrentAudioFrame();
	m_Impl->m_CurrentAudioFrameFlowOutSubj.get_subscriber().on_next(aframe);
}
///	\brief pause playback
void PlayerModel::Pause()
{

}
///	\brief stop playeback
void PlayerModel::Stop()
{

}
///	\brief set frame rate
///	\param	fps	the frame rate to set
void PlayerModel::SetFrameRate(int fps)
{
	
}
///	\brief get the current frame
///	\return the current frame
rxcpp::observable<PlayerModel::vframe_buffer_t> PlayerModel::VideoFrameFlowOut()
{
	return m_Impl->m_CurrentVideoFrameFlowOutSubj.get_observable().as_dynamic();
}
///	\brief get the current frame
///	\return the current frame
rxcpp::observable<PlayerModel::aframe_buffer_t> PlayerModel::AudioFrameFlowOut()
{
	return m_Impl->m_CurrentAudioFrameFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion