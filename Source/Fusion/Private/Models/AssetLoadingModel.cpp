#include <Models/AssetLoadingModel.h>
#include <Core/AssetTypeResolver.h>
#include <LoadObj.h>
#include <LoadPly.h>
#include <plog/Log.h>

namespace fu {
namespace fusion {

struct AssetLoadingModel::Impl
{
	rxcpp::subjects::subject<std::string>	m_FilepathFlowInSubj;
	rxcpp::subjects::subject<io::MeshData>	m_MeshDataFlowOutSubj;
	/// Construction
	Impl() { }
};
/// Construction
AssetLoadingModel::AssetLoadingModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ 
	///=======================
	/// Filepath flow in task
	///=======================
	m_Impl->m_FilepathFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::string& filepath) 
	{
		LOG_INFO << "Attempting to load file: " << filepath;
		AssetType type = AssetTypeResolver::GetAssetType(filepath);
		io::MeshData data;

		switch (type)
		{
		case fu::fusion::AssetType::Unknown:
			return;
		case fu::fusion::AssetType::Ply:
			data = io::LoadPly(filepath);
			break;
		case fu::fusion::AssetType::Obj:
			data = io::LoadObj(filepath);
			break;
		default:
			LOG_WARNING << "Not supported file type: " << filepath;
			return;
		}
		m_Impl->m_MeshDataFlowOutSubj.get_subscriber().on_next(data);
	});
}

rxcpp::observer<std::string> fu::fusion::AssetLoadingModel::FilepathFlowIn()
{
	return m_Impl->m_FilepathFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<io::MeshData> fu::fusion::AssetLoadingModel::MeshDataFlowOut()
{
	return m_Impl->m_MeshDataFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu