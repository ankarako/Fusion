#ifndef	__FUSION_PUBLIC_MODELS_PLAYER_MODEL_H__
#define __FUSION_PUBLIC_MODELS_PLAYER_MODEL_H__

#include <Initializable.h>
#include <Destroyable.h>
#include <Buffer.h>
#include <spimpl.h>
#include <string>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {
class Coordination;
class SettingsRepo;
/// \class PlayerModel
///	\brief Fusion Model that controls media file loading and playback
class PlayerModel	: public app::Initializable, public app::Destroyable
{
public:
	using coord_ptr_t = std::shared_ptr<Coordination>;
	using frame_t = BufferCPU<uchar4>;
	using srepo_ptr_t = std::shared_ptr<SettingsRepo>;
	///	Construction
	///	\brief default contstructor
	///	does not depend on anything (yet)
	PlayerModel(coord_ptr_t coord, srepo_ptr_t srepo);
	/// \brief initialize the model
	///	Creates an internal decoding context
	void Init() override;
	///	\brief destroy the model
	///	destroys the internal decoding context
	void Destroy() override;
	///	\brief load a media file
	///	\param	filepath	the path to the file to load
	void LoadFile(const std::string& filepath);
	///	\brief start playback
	void Start();
	///	\brief pause playback
	void Pause();
	///	\brief stop playback
	void Stop();
	///	\brief seek to a specified frame position
	///	\param	framePos	the frame position to seek
	void Seek(int framePos);
	///	\brief seek forward one frame
	void SeekForward();
	///	\brief seek backward one frame
	void SeekBackward();
	///	\brief set the number of frames to prefetch
	///	\param	frameCount	the number of frames to prefetch
	void SetFramePrefetchCount(size_t frameCount);
	///	\brief set scaling size
	void SetScalingSize(uint2 size);
	/// \brief checj if the model has opened a file
	///	\return true if the video player is opened
	bool IsOpen() const;

	uint2 GetFrameSize() const;

	const BufferCPU<uchar4>& GetCurrentFrame();
	///	\brief current frame id output
	rxcpp::observable<size_t> CurrentFrameIdFlowOut();
	///	\brief frame duration output
	rxcpp::observable<size_t> FrameCountFlowOut();
	///	\brief frame buffer output
	rxcpp::observable<frame_t> CurrentFrameFlowOut();
	///	\brief frame width output event
	rxcpp::observable<int>	FrameWidthFlowOut();
	///	\brief frame height output event
	rxcpp::observable<int>	FrameHeightFlowOut();
	///	\brief frame size flow out
	rxcpp::observable<uint2> FrameSizeFlowOut();

	rxcpp::observable<void*> OnVideoLoaded();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class VideoPlayerModel
}	///	!namespac e fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_PLAYER_MODEL_H__