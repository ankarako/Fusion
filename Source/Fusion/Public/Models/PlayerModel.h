#ifndef	__FUSION_PUBLIC_MODELS_PLAYERMODEL_H__
#define __FUSION_PUBLIC_MODELS_PLAYERMODEL_H__

#include <Buffer.h>
#include <Initializable.h>
#include <Destroyable.h>
#include <spimpl.h>
#include <string>
#include <vector_types.h>
#include <rxcpp/rx.hpp>

namespace fusion {
///	\class PlayerModel
///	\brief Video Player Logic
class PlayerModel : public app::Initializable, public app::Destroyable
{
public:
	using vframe_buffer_t = fu::Buffer<float3, fu::BufferStorageProc::CPU>;
	using aframe_buffer_t = fu::Buffer<float, fu::BufferStorageProc::CPU>;
	///	Construction
	PlayerModel();
	///	Destruction
	~PlayerModel();
	///	\brief initialization
	void Init() override;
	///	\brief destroy the player's state
	void Destroy() override;
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
	///	\brief set video width
	///	\brief set video height
	///	\brief get the current frame
	///	\return the current frame
	rxcpp::observable<vframe_buffer_t> VideoFrameFlowOut();
	///	\brief get the current frame
	///	\return the current frame
	rxcpp::observable<aframe_buffer_t> AudioFrameFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class VideoPlayerModel
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_MODELS_PLAYERMODEL_H__