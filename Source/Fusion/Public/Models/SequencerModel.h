#ifndef __FUSION_PUBLIC_MODELS_SEQUENCERMODEL_H__
#define __FUSION_PUBLIC_MODELS_SEQUENCERMODEL_H__

#include <spimpl.h>

namespace fu {
namespace fusion {
///
class PlayerModel;
class AnimationModel;

class SequencerModel
{
public:
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class SequencerModel
}
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_SEQUENCERMODEL_H__