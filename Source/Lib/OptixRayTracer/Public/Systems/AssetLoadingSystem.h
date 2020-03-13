#ifndef	__OPTIXRAYTRACER_PUBLIC_SYSTEMS_ASSETLOADINGSYSTEM_H__
#define	__OPTIXRAYTRACER_PUBLIC_SYSTEMS_ASSETLOADINGSYSTEM_H__

#include <Components/TriangleMeshComp.h>
#include <Components/AnimatedTriangleMeshComp.h>
#include <Components/PointCloudComp.h>
#include <Components/ContextComp.h>
#include <string>
#include <rxcpp/rx.hpp>
#include <spimpl.h>

namespace fu {
namespace rt {

///	\class AssetLoadingSystem
///	\brief provides asset loading functionalities
class AssetLoadingSystem
{
public:
	AssetLoadingSystem();
	///	\brief load an asset
	///	top level loading function. It determines the asset type and
	///	\param	filepath	the path to the file to load
	void LoadAsset(const std::string& filepath, ContextComp& ctxComp);
	/// outputs
	/// \brief	Triangle mesh output
	///	if the system is called to load an asset, and determines that
	///	it is a triangle mesh, it will come out frome here
	rxcpp::observable<TriangleMeshComp>		TriangleMeshFlowOut();
	rxcpp::observable<PointCloudComp>		PointCloudFlowOut();
	///
	// rxcpp::observable<AnimatedTrangleMeshComp>
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class AssetLoadingSystem
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_ASSETLOADINGSYSTEM_H__