#ifndef	__FUSION_PUBLIC_MODELS_PERFCAPPLAYERMODEL_H__
#define __FUSION_PUBLIC_MODELS_PERFCAPPLAYERMODEL_H__

#include <MeshData.h>
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
	/// 
	rxcpp::observer<std::string> PerfcapFilepathFlowIn();

	rxcpp::observable<io::MeshData> TemplateMeshDataFlowOut();
	rxcpp::observable<SequenceItem>	SequenceItemFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PerfcapPlayerModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_PERFCAPPLAYERMODEL_H__