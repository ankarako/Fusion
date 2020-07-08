#ifndef __FUSION_PUBLIC_MODELS_FUSEDEXPORTMODEL_H__
#define __FUSION_PUBLIC_MODELS_FUSEDEXPORTMODEL_H__

#include <Buffer.h>
#include <Core/SequenceItem.h>
#include <Initializable.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>
#include <string>

namespace fu {
namespace fusion {
class FusedExportModel : public app::Initializable
{
public:
	FusedExportModel();
	void Init() override;
	rxcpp::observable<int>				SeekFrameIdFlowOut();
	rxcpp::observer<BufferCPU<uchar4>>	FusedFrameFlowIn();
	rxcpp::observer<int>				AnimatedFramesCountFlowIn();
	rxcpp::observer<std::string>		ExportFilepathFlowIn();
	rxcpp::observer<std::string>		ExportMentorLayerFilepathFlowIn();
	rxcpp::observer<uint2>				VideoSizeFlowIn();
	rxcpp::observable<void*>			StartedExportingFlowOut();
	rxcpp::observable<void*>			StartedExportingMentorLayer();
	rxcpp::observable<void*>			FinishedExportingFlowOut();
	rxcpp::observable<void*>			FinishedExportingMentorLayerFlowOut();
	rxcpp::observer<SequenceItem>		SequenceItemFlowIn();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FusedExportModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_FUSEDEXPORTMODEL_H__