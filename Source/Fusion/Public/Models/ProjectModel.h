#ifndef	__FUSION_PUBLIC_MODELS_PROJECTMODEL_H__
#define	__FUSION_PUBLIC_MODELS_PROJECTMODEL_H__

#include <Initializable.h>
#include <Core/DirEntry.h>
#include <string>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {
///	
class SettingsRepo;
///	\class WorkspaceModel
///	\brief simple workspace util
class ProjectModel : public app::Initializable
{
public:
	using srepo_ptr_t = std::shared_ptr<SettingsRepo>;
	/// Construction
	ProjectModel(srepo_ptr_t srepo);
	/// brief create a new project
	void CreateProject(const std::string& path);
	void Init() override;
	///
	void Save();
	/// 
	void LoadProject(const std::string& filepath);
	const std::string& WorkSpaceDirectory() const;
	///
	bool IsProjectSaved() const;

	rxcpp::observer<DirEntry>	CreateNewProjectFlowIn();
	rxcpp::observer<DirEntry>	LoadProjectFlowIn();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class WorkSpaceModel
}	///	!namespace fusion
}	///	!namesapce fu
#endif	///	!__FUSION_PUBLIC_MODELS_PROJECTMODEL_H__