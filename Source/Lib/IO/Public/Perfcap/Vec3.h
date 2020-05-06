#ifndef	__IO_PUBLIC_PERFCAP_VEC3_H__
#define __IO_PUBLIC_PERFCAP_VEC3_H__

namespace fu {
namespace io {

template <typename T>
struct Vec3
{
	T x;
	T y;
	T z;

	Vec3()
		: x(T(0)), y(T()), z(T())
	{ }

	Vec3(T _x, T _y, T _z)
		: x(_x), y(_y), z(_z)
	{ }

	template <typename F>
	void serialize(F& buf) const
	{
		buf << x << y << z;
	}

	template <typename F>
	void parse(F& buf)
	{
		buf >> x >> y >> z;
	}

	bool operator==(const Vec3<T>& other) const 
	{
		return x == other.x && y == other.y && z == other.z;
	}
};
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAP_VEC3_H__