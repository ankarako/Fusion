#include <Utils/Arcball.h>
#include <optix_world.h>
#include <Utils/Quaternion.h>

namespace fu {
namespace rt {

void fu::rt::Arcball::Rotate(float prevx, float prevy, float curx, float cury, float weight, array_t& m)
{
	float a[3];
	float b[3];
	ToSphere(prevx, prevy, a);
	ToSphere(curx, cury, b);

	optix::float3 _a = optix::make_float3(a[0], a[1], a[2]);
	optix::float3 _b = optix::make_float3(b[0], b[1], b[2]);
	Quaternion q(_a, _b);
	q.normalize();
	optix::Matrix4x4 mat = q.rotationMatrix();
	std::memcpy(m.data(), mat.getData(), 16 * sizeof(float));
}

void fu::rt::Arcball::Translate(float prevx, float prevy, float curx, float cury, vec_t& m)
{
	float a[3];
	float b[3];
	ToVector(prevx, prevy, a);
	ToVector(curx, cury, b);
	optix::float3 trans = optix::make_float3(a[0] - b[0], a[2] - b[2], a[1] - b[1]);
	std::memcpy(m.data(), &trans, 3 * sizeof(float));
}

void fu::rt::Arcball::Zoom(float prevx, float prevy, float curx, float cury, array_t& m)
{

}

void fu::rt::Arcball::ToSphere(const float x, const float y, float res[3])
{
	float _x = (x - m_CenterX) / m_Radius;
	float _y = (1.0f - y - m_CenterY) / m_Radius;
	float _z = 0.0f;

	float len2 = _x * _x + _y * _y;
	if (len2 > 1.0f)
	{
		/// project to closest point on edge of sphere
		float len = sqrtf(len2);
		_x /= len;
		_y /= len;
	}
	else
	{
		_z = sqrtf(1.0f - len2);
	}
	res[0] = _x;
	res[1] = _y;
	res[2] = _z;
}

void fu::rt::Arcball::ToVector(const float x, const float y, float v[3])
{
	float d = x * x + y * y;
	float sqRad = m_Radius * m_Radius;
	if (d > sqRad)
	{
		v[0] = x;
		v[1] = y;
		v[2] = 0.0f;
	}
	else
	{
		v[0] = x;
		v[1] = y;
		v[2] = sqrtf(sqRad - d);
	}
}
}	///	!namespace rt
}	///	!namespace fu