#ifndef	__FUSION_PUBLIC_PRESENTERS_FUIMPORTPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_FUIMPORTPRESENTER_H__

#include <Initializable.h>
#include <Core/Coordination.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {

class FuImportModel;
class ProjectModel;
class FileExplorerView;

class FuImportPresenter : public app::Initializable
{
public:
	using prj_model_ptr_t 	= std::shared_ptr<ProjectModel>;
	using model_ptr_t 		= std::shared_ptr<FuImportModel>;
	using fexp_view_ptr_t	= std::shared_ptr<FileExplorerView>;
	using coord_ptr_t		= std::shared_ptr<Coordination>;

	FuImportPresenter(model_ptr_t model, prj_model_ptr_t prj_model, fexp_view_ptr_t fexp_view, coord_ptr_t coord);
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FuImportPresenter
}	///	!namesapce fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_FUIMPORTPRESENTER_H__