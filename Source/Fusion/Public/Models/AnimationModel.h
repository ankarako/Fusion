#ifndef __FUSION_PUBLIC_MODELS_ANIMATIONMODEL_H__
#define __FUSION_PUBLIC_MODELS_ANIMATIONMODEL_H__

#include <Perfcap/AnimationFrame.h>
#include <Perfcap/TemplateMesh.h>
#include <MeshData.h>
#include <Initializable.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {


class AnimationModel : public app::Initializable
{
public:
	AnimationModel();
	void Init() override;
	rxcpp::observer<io::AnimationFrame>			AnimationFrameFlowIn();
	rxcpp::observer<io::TemplateMesh>			TemplateMeshFlowIn();
	rxcpp::observable<io::MeshData>				MeshDataFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_ANIMATIONMODEL_H__