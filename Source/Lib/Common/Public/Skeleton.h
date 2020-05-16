#ifndef __COMMON_PUBLIC_SKINNINGDATA_H__
#define __COMMON_PUBLIC_SKINNINGDATA_H__

#include <vector>

namespace fu {
///	\struct PerfJoint
///	\brief a (perfcap) joint
struct PerfJoint 
{
	int Id;						///<	The joint id
	int ParentId;				///<	The joint's parent id
	std::vector<int> Children;	///<	The joint's children
	std::vector<int> Hierarchy;	///<	the joint hierarchy (this joint's parents)

	float Swing1[3];	///<	Swing1 vec
	float Swing2[3];
	float Twist[3];
};	///	!struct PerfJoint
///	\struct PerfSkinningData
///	\brief Skinning Data
struct PerfSkinningData
{
	using weight_array = std::vector<float>;

	int NumVertices;
	int NumWeightPerVertex;
	std::vector<weight_array> Weights;
};	///	!SkinningData
}	///	!namespace fu
#endif	///	!__COMMON_PUBLIC_SKINNINGDATA_H__