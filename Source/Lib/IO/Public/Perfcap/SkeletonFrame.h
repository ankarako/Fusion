#ifndef __IO_PUBLIC_PERFCAP_SKELETONFRAME_H__
#define __IO_PUBLIC_PERFCAP_SKELETONFRAME_H__

#include <Perfcap/Vec3.h>
#include <vector>

namespace fu {
namespace io {

struct SkeletonFrame
{
	Vec3<float>					GlobalRootTranslation;
	std::vector<Vec3<float>> 	JointRotations;

	void reset()
	{
		GlobalRootTranslation = Vec3<float>();
		JointRotations.clear();
	}

	template <typename F>
	void serialilze(F& buf) const
	{
		buf << GlobalRootTranslation << JointRotations;
	}

	template <typename F>
	void parse(F& buf)
	{
		buf >> GlobalRootTranslation >> JointRotations;
	}

	bool operator==(const SkeletonFrame& other) const
	{
		return GlobalRootTranslation == other.GlobalRootTranslation
			&& JointRotations == other.JointRotations;
	}
};	///	!struct SkeletonFrame
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAP_SKELETONFRAME_H__