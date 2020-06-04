#ifndef __MVT_PUBLIC_MODELS_PERFORMACEIMPORTMODEL_H__
#define __MVT_PUBLIC_MODELS_PERFORMACEIMPORTMODEL_H__

#include <Initializable.h>
#include <Destroyable.h>
#include <PerfcapAnimationData.h>
#include <TrackedData.h>
#include <VolcapCameraData.h>
#include <MeshData.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>
#include <string>

namespace fu {
namespace mvt {
///	\class PerformanceImportModel
///	\brief imports performance files
class PerformanceImportModel : public app::Initializable, public app::Destroyable
{
public:
	/// Construction
	///	\brief no dependencies
	PerformanceImportModel();
	/// \brief Initialize the model
	void Init() override;
	///	\brief destroy the model
	void Destroy() override;

	std::string TempFolderPath() const;
	std::string SkinningFilename() const;
	std::string SkeletonFilename() const;
	std::string TrackedParamsFilename() const;
	std::string TemplateMeshFilename() const;
	/// inputs
	///	\brief filepath input for importing
	rxcpp::observer<std::string> ImportFilepathFlowIn();
	/// outputs	
	rxcpp::observable<std::string>								TemplateMeshFilepathFlowOut();
	rxcpp::observable<std::vector<std::string>>					VideoTextureFilepathsFlowOut();
	rxcpp::observable<std::string>								TextureAtlasFilepathFlowOut();
	rxcpp::observable<io::perfcap_skeleton_ptr_t>				SkeletonFlowOut();
	rxcpp::observable<io::perfcap_skin_data_ptr_t>				SkinDataFlowOut();
	rxcpp::observable<std::vector<io::volcap_cam_data_ptr_t>>	ViewportDataFlowOut();
	rxcpp::observable<io::tracked_seq_ptr_t>					TrackedSequenceDataFlowOut();
	rxcpp::observable<io::MeshData>								MeshDataFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PerformanceImportModel
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_MODELS_PERFORMACEIMPORTMODEL_H__