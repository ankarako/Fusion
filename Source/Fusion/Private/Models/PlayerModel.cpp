#include <Models/PlayerModel.h>
#include <Transcoder.h>

namespace fusion {
///	\struct Impl
///	\brief	PlayerModel implementation
struct PlayerModel::Impl
{
	using transcoder_ptr_t = std::shared_ptr<video::Transcoder>;
	transcoder_ptr_t	m_Transcoder;
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
PlayerModel::~PlayerModel() { }
///	\brief Initialization
///
void PlayerModel::Init()
{
	m_Impl->m_Transcoder->Initialize();
}
///	\brief open a video file
void PlayerModel::LoadFile(const std::string& filepath)
{
	return;
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
}	///	!namespace fusion