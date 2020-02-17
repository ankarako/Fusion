#ifndef	__FUSION_PUBLIC_MODELS_PLAYERMODEL_H__
#define __FUSION_PUBLIC_MODELS_PLAYERMODEL_H__

#include <Initializable.h>
#include <spimpl.h>
#include <string>

namespace fusion {
///	\class PlayerModel
///	\brief Video Player Logic
class PlayerModel : public app::Initializable
{
public:
	///	Construction
	PlayerModel();
	///	Destruction
	~PlayerModel();
	///	\brief initialization
	void Init() override;
	///	\brief open a video file
	void LoadFile(const std::string& filepath);
	///	\brief get the loaded video's frame count
	///	\return the video's frame count
	size_t GetFrameCount() const;
	///	\brief get the current frame in the loaded frame
	///	\return	the current frame index
	size_t GetCurrentFrameIndex() const;
	///	\brief go to specific frame in the video
	///	\param	index	the frame index to seek
	void SeekFrame(size_t index);
	///	\brief start playback
	void Play();
	///	\brief pause playback
	void Pause();
	///	\brief stop playeback
	void Stop();
	///	\brief set frame rate
	///	\param	fps	the frame rate to set
	void SetFrameRate(int fps);
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class VideoPlayerModel
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_MODELS_PLAYERMODEL_H__