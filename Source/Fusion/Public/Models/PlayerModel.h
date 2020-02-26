#ifndef	__FUSION_PUBLIC_MODELS_PLAYER_MODEL_H__
#define __FUSION_PUBLIC_MODELS_PLAYER_MODEL_H__

#include <Initializable.h>
#include <Destroyable.h>
#include <spimpl.h>
#include <string>

namespace fu {
namespace fusion {
class PlayerModel	: public app::Initializable, public app::Destroyable
{
public:
	PlayerModel();
	void Init() override;
	void Destroy() override;
	void LoadFile(const std::string& filepath);
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class VideoPlayerModel
}	///	!namespac e fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_PLAYER_MODEL_H__