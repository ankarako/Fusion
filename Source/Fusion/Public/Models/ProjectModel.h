#ifndef	__FUSION_PUBLIC_MODELS_PROJECTMODEL_H__
#define	__FUSION_PUBLIC_MODELS_PROJECTMODEL_H__

#include <string>
#include <spimpl.h>

namespace fu {
namespace fusion {
///	
class SettingsRepo;
///	\class WorkspaceModel
///	\brief simple workspace util
class ProjectModel
{
public:
	/// Construction
	ProjectModel();
	/// brief create a new project
	void CreateProject(const std::string& directory);
	///
	const std::string& WorkSpaceDirectory(); const
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class WorkSpaceModel
}	///	!namespace fusion
)	///	!namesapce fu
#endif	///	!__FUSION_PUBLIC_MODELS_PROJECTMODEL_H__