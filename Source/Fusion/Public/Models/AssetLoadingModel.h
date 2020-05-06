#ifndef	__FUSION_PUBLIC_MODELS_ASSETLOADINGMODEL_H__
#define __FUSION_PUBLIC_MODELS_ASSETLOADINGMODEL_H__

#include <string>
#include <spimpl.h>
#include <MeshData.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {

class AssetLoadingModel
{
public:
	AssetLoadingModel();

	rxcpp::observer<std::string>	FilepathFlowIn();
	rxcpp::observable<io::MeshData> MeshDataFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class AssetLoadingModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_ASSETLOADINGMODEL_H__