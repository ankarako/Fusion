#ifndef __OPTIXRAYTRACER_PUBLIC_UTILS_ARCBALL_H__
#define __OPTIXRAYTRACER_PUBLIC_UTILS_ARCBALL_H__

#include <array>

namespace fu {
namespace rt {
///	\class Arcball
///	\brief 
class Arcball
{
public:
	using array_t = std::array<float, 16>;
	Arcball() { }
	static void Rotate(float prevx, float prevy, float curx, float cury, float weight, array_t& m);
	static void Translate(float prevx, float prevy, float curx, float cury, array_t& m);
	static void Zoom(float prevx, float prevy, float curx, float cury, array_t& m);
private:
	static void ToSphere(const float x, const float y, float res[3]);
	static void ToRotVector(const float x, const float y, float v[3]);
	static constexpr const float m_CenterX = 0.0f;
	static constexpr const float m_CenterY = 0.0f;
	static constexpr const float m_Radius = 10.0f;
};	///	!class Arcballrtapp
}	/// !namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_UTILS_ARCBALL_H__