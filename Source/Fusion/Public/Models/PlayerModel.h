#ifndef	__FUSION_PUBLIC_MODELS_PLAYER_MODEL_H__
#define __FUSION_PUBLIC_MODELS_PLAYER_MODEL_H__

#include <Initializable.h>
#include <Destroyable.h>
#include <spimpl.h>
#include <string>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {
/// \class PlayerModel
///	\brief Fusion Model that controls media file loading and playback
class PlayerModel	: public app::Initializable, public app::Destroyable
{
public:
	///	Construction
	///	\brief default contstructor
	///	does not depend on anything (yet)
	PlayerModel();
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
	///	\brief current frame id output
	rxcpp::observable<size_t> CurrentFrameIdFlowOut();
	///	\brief frame duration output
	rxcpp::observable<size_t> FrameCountFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class VideoPlayerModel
}	///	!namespac e fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_PLAYER_MODEL_H__