#ifndef	__IO_PUBLIC_PERFCAP_SKELETON_H__
#define __IO_PUBLIC_PERFCAP_SKELETON_H__

#include <Perfcap/Vec3.h>
// The skinned avatar is fully defined by the above three files (a), (b) and (c).
// It consists of 18 joints, defined as follows								
//          										3 Head
//  0 - Root (Global RT)							|
//  1 - Middle Torso			                    | 
//  2 - Down Torso    			     R.Shoulder ___ 0 ___  L.Shoulder
//  3 - Head								   12   |    15     
//  4 - R.Hip		   				          /     |     \
//  5 - R.Knee								 /	    |      \
//  6 - R.Ankle		  					   / 	Mid 1 Tor    \
//  7    								  /         |		  \		
//  8 - L.Hip    				R.Elbow 13     Down	2 Tor 	   16 L.Elbow
//  9 - L.Knee    					 	|          / \          |
// 10 - L.Ankle    					   |    R.Hip 4   8 L.Hip    |
// 11 	 				   	  R.Hand 14          /     \          17 L.Hand
// 12 - R.Shoulder							    /       \   
// 13 - R.Elbow 						R.Knee 5         9 L.Knee 
// 14 - R.Hand								   |         |
// 15 - L.Shoulder							   |         |
// 16 - L.Elbow							       |     	 |		
// 17 - L.Hand						   R.Ankle 6         10 L.Ankle
// 
namespace fu {
namespace io {

enum class JointName : int
{
	Root 		= 0,
	MiddleTorso = 1,
	DownTorso	= 2,
	Head		= 3,
	RHip		= 4,
	RKnee		= 5,
	RAnkle		= 6,
	RFoot 		= 7,
	LHip 		= 8,
	LKnee		= 9,
	LAnkle 		= 10,
	LFoot		= 11,
	RShoulder	= 12,
	RElbow		= 13,
	RHand		= 14,
	LShoulder	= 15,
	LElbow		= 16,
	LHand		= 17,
	JointCount
};

struct SkeletonJoint
{
	JointName 	Name;
	JointName	Parent;
	Vec3<float>	Position;

	template <typename F>
	void serialize(F& buf) const
	{
		buf << (int)Name << (int)parent << Position;
	}

	template <typename F>
	void parse(F& buf)
	{
		int n, p;
		buf >> n >> p >> Position;
		Name = (JointName)n;
		Parent = (JointName)p;
	}

	bool operator==(const SkeletonJoint& other) const
	{
		return Name == other.Name
			&& Parent == other.Parent
			&& Position == other.Position;
	}
};
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAP_SKELETON_H__