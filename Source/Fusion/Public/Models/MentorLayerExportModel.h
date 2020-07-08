#ifndef __FUSION_PUBLIC_MODELS_MENTORLAYEREXPORTMODEL_H__
#define __FUSION_PUBLIC_MODELS_MENTORLAYEREXPORTMODEL_H__

#include <spimpl.h>

namespace fu {
namespace fusion {
class MentorLayerExportModel
{
public:
	MentorLayerExportModel();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class MentorLayerExportModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_MENTORLAYEREXPORTMODEL_H__