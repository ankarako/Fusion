#ifndef __IO_PUBLIC_PERFCAP_TEMPLATEMESH_H__
#define __IO_PUBLIC_PERFCAP_TEMPLATEMESH_H__

#include <Perfcap/Vec3.h>
#include <Perfcap/Skeleton.h>
#include <vector>

namespace fu {
namespace io {

struct TemplateMesh
{
	std::vector<Vec3<float>>	Vertices;
	std::vector<Vec3<float>>	Colors;
	std::vector<Vec3<float>>	Normals;
	std::vector<Vec3<int>>		Faces;

	std::vector<SkeletonJoint>		Skeleton;
	std::vector<std::vector<float>>	SkinningWeights;

	template <typename F>
	void serialize(F& buf) const
	{
		buf << Vertices << Colors << Normals << Faces << Skeleton << SkinningWeights;
	}

	template <typename F>
	void parse(F& buf)
	{
		buf >> Vertices >> Colors >> Normals >> Faces >> Skeleton >> SkinningWeights;
	}

	bool operator==(const TemplateMesh& other) const
	{
		return Vertices == other.Vertices
			&& Colors == other.Colors
			&& Normals == other.Normals
			&& Faces == other.Faces
			&& Skeleton == other.Skeleton
			&& SkinningWeights == other.SkinningWeights;
	}
};	///	!struct TemplateMesh
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAP_TEMPLATEMESH_H__