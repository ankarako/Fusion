#ifndef	__FUSION_PUBLIC_MODELS_WORKSPACEMODEL_H__
#define	__FUSION_PUBLIC_MODELS_WORKSPACEMODEL_H__

#include <spimpl.h>

namespace fu {
namespace fusion {
///	
class SettingsRepo;
///	\class WorkspaceModel
///	\brief simple workspace util
class WorkSpaceModel
{
public:

private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class WorkSpaceModel
}	///	!namespace fusion
)	///	!namesapce fu
#endif	///	!__FUSION_PUBLIC_MODELS_WORKSPACEMODEL_H__