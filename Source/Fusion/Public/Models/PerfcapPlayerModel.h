#ifndef	__FUSION_PUBLIC_MODELS_PERFCAPPLAYERMODEL_H__
#define __FUSION_PUBLIC_MODELS_PERFCAPPLAYERMODEL_H__

#include <MeshData.h>
#include <Perfcap/TemplateMesh.h>
#include <Perfcap/AnimationFrame.h>
#include <Core/SequenceItem.h>
#include <string>
#include <rxcpp/rx.hpp>
#include <spimpl.h>

namespace fu {
namespace fusion {

class PerfcapPlayerModel
{
public:
	/// Construction
	PerfcapPlayerModel();

	void Start();
	void Pause();
	void Stop();
	void SeekFrame(size_t frameId);
	void SeekForward();
	void SeekBackward();
	/// 
	rxcpp::observer<std::string>				PerfcapFilepathFlowIn();
	rxcpp::observable<io::MeshData>				TemplateMeshDataFlowOut();
	rxcpp::observable<SequenceItem>				SequenceItemFlowOut();
	rxcpp::observable<io::TemplateMesh>			TemplateMeshFlowOut();
	rxcpp::observable<io::AnimationFrame>		AnimationFrameFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PerfcapPlayerModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_PERFCAPPLAYERMODEL_H__