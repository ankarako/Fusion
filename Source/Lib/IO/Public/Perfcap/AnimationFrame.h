#ifndef __IO_PUBLIC_PERFCAP_ANIMATIONFRAME_H__
#define __IO_PUBLIC_PERFCAP_ANIMATIONFRAME_H__

#include <Perfcap/Vec3.h>
#include <Perfcap/SkeletonFrame.h>
#include <Perfcap/TextureFrame.h>
#include <vector>

namespace fu {
namespace io {

struct AnimationFrame
{
	int 			FrameIndex;
	SkeletonFrame	Skeleton;
	TextureFrame	Texture;

	void reset()
	{
		FrameIndex = -1;
		Skeleton.reset();
		Texture.reset();
	}

	template <typename F>
	void serialize(F& buf) const
	{
		buf << FrameIndex << Skeleton << Texture;
	}

	template <typename F>
	void parse(F& buf)
	{
		buf >> FrameIndex >> Skeleton >> Texture;
	}

	bool operator==(const AnimationFrame& other) const
	{
		return FrameIndex == other.FrameIndex
			&& Skeleton == other.Skeleton
			&& Texture == other.Texture;
	}

	static bool compare_by_index(const AnimationFrame& lhs, const AnimationFrame& rhs)
	{
		return lhs.FrameIndex < rhs.FrameIndex;
	}
};	///	!struct AnimationFrame
}	/// !namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAP_ANIMATIONFRAME_H__