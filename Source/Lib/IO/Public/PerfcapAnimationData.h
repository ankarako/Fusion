#ifndef __IO_PUBLIC_PERFCAPANIMATIONDATA_H__
#define __IO_PUBLIC_PERFCAPANIMATIONDATA_H__

#include <vector>
#include <array>
#include <Buffer.h>
#include <memory>

namespace fu {
namespace io {
///	\struct PerfcapJoint
///	\brief a perfcap skeleton joint
struct PerfcapJoint
{
	int 					Id;
	int 					ParentId;
	std::vector<int> 		Children;
	std::vector<int>		Hierarchy;
	std::array<float, 3>	Swing1;
	std::array<float, 3>	Swing2;
	std::array<float, 3>	Twist;
	std::array<float, 3>	Position;
};	///	!struct PerfcapJoint
///	\typedef perfcap_joint_ptr_t
///	\brief a shared PerfcapJoint
using perfcap_joint_ptr_t = std::shared_ptr<PerfcapJoint>;
///	\brief create a perfcapp joint ptr
static perfcap_joint_ptr_t CreatePerfcapJoint()
{
	return std::make_shared<PerfcapJoint>();
}
///	\typedef PerfcapSkeleton
///	\brief a vector of PerfcapJoints
using PerfcapSkeleton = std::vector<perfcap_joint_ptr_t>;
///	\typedef perfcap_skeleton_ptr_t
///	\brief a shared PerfcapSkeleton
using perfcap_skeleton_ptr_t = std::shared_ptr<PerfcapSkeleton>;
///	\brief create a perfcap skeleton ptr
static perfcap_skeleton_ptr_t CreatePerfcapSkeleton()
{
	return std::make_shared<PerfcapSkeleton>();
}
///	\struct SkinningData
///	\brief A mesh's skinning weigts
struct SkinningData
{
	int 				NumVertices;
	int 				NumWeightsPerVertex;
	std::vector<float> 	WeightData;
	std::vector<int>	Jointdata;
};
///	\typedef perfcap_skin_data_ptr_t 
///	\brief a shared SkinningData object
using perfcap_skin_data_ptr_t = std::shared_ptr<SkinningData>;
///	\brief create a perfcap_skin_data_ptr_t  object
static perfcap_skin_data_ptr_t CreatePerfcapSkinData()
{
	return std::make_shared<SkinningData>();
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAPANIMATIONDATA_H__