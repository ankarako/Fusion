#ifndef	__FUSION_PUBLIC_CORE_DEPTHESTIMATIONMODELS_H__
#define __FUSION_PUBLIC_CORE_DEPTHESTIMATIONMODELS_H__

#include <string>
#include <array>
#include <map>

namespace fu {
namespace fusion {
enum class DepthEstimationModels : unsigned int
{
	UResNet = 0,
	RectNet,
	SVSSuper,
	SVSLR,
	SVSUD,
	SVSTR,
	ModelCount
};	///	!enum DepthEstimationModels

static const std::vector<DepthEstimationModels> k_DepthEstimationModels =
{
	DepthEstimationModels::UResNet,
	DepthEstimationModels::RectNet,
	DepthEstimationModels::SVSSuper,
	DepthEstimationModels::SVSLR,
	DepthEstimationModels::SVSUD,
	DepthEstimationModels::SVSTR
};

static const std::map<DepthEstimationModels, std::string> DepthEstModel2Str =
{
	{ DepthEstimationModels::UResNet, "UResNet" },
	{ DepthEstimationModels::RectNet, "RectNet" },
	{ DepthEstimationModels::SVSSuper, "Spherical View Synthesis - Supervised" },
	{ DepthEstimationModels::SVSLR, "Spherical View Synthesis - Left/Right" },
	{ DepthEstimationModels::SVSUD, "Spherical View Synthesis - Up/Down" },
	{ DepthEstimationModels::SVSTR, "Spherical View Synthesis - Trinocular" },
};
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_CORE_DEPTHESTIMATIONMODELS_H__