#ifndef __FUSION_PUBLIC_MODELS_FUIMPORTMODEL_H__
#define __FUSION_PUBLIC_MODELS_FUIMPORTMODEL_H__

#include <Initializable.h>
#include <MeshData.h>
#include <PerfcapSkeletonImport.h>
#include <PerfcapSkinningDataImport.h>
#include <PerfcapTrackedParamsImport.h>
#include <Settings/ProjectSettings.h>
#include <Settings/FuImportSettings.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>
#include <string>
#include <tuple>

namespace fu {
namespace fusion {

class SettingsRepo;

class FuImportModel : public app::Initializable
{
public:
	using prj_settings_ptr_t	= std::shared_ptr<ProjectSettings>;
	using settings_ptr_t		= std::shared_ptr<FuImportSettings>;
	using perfcap_tex_mesh_t	= std::tuple<io::MeshData, io::perfcap_skeleton_ptr_t, io::perfcap_skin_data_ptr_t, io::tracked_seq_ptr_t, std::string, std::string>;
	using srepo_ptr_t			= std::shared_ptr<SettingsRepo>;

	FuImportModel(srepo_ptr_t srepo);
	void Init() override;
	rxcpp::observer<std::string>			FuFilepathFlowIn();
	rxcpp::observer<prj_settings_ptr_t>		PrjSettingsFlowIn();

	rxcpp::observable<perfcap_tex_mesh_t>	PerfcapMeshDataFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FuImportModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_FUIMPORTMODEL_H__