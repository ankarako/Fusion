#ifndef	__OPTIXRAYTRACER_PUBLIC_UTILS_TRACKBALL_H__
#define __OPTIXRAYTRACER_PUBLIC_UTILS_TRACKBALL_H__

#include <optix_world.h>
#include <Components/RaygenProgComp.h>
#include <algorithm>

namespace fu {
namespace rt {
///	\class Trackball
///	\brief a trackball for manipulating cameras
class Trackball
{
public:
	enum ViewMode
	{
		EyeFixed,
		LookAtFixed,
	};

	void StartTracking(int x, int y)
	{
		m_PrevPosX = x;
		m_PrevPosY = y;
		m_PerformTracking = true;
	}

	void UpdateTracking(int x, int y, int /*canvasWidth*/, int /*canvasHeight*/)
	{
		int deltaX = x - m_PrevPosX;
		int deltaY = y - m_PrevPosX;

		m_PrevPosX = x;
		m_PrevPosY = y;

		m_Latitude = radians(std::min(89.0f, std::max(-89.0f, degrees(m_Latitude) + 0.5f * deltaY)));
		m_Longitude = radians(fmod(degrees(m_Longitude) - 0.5f * deltaX, 360.0f));

		if (!m_GimbalLock)
		{

		}
	}

	void UpdateRaygenComp(RaygenProgComp& comp)
	{
		optix::float3 localDir;
		localDir.x = cos(m_Latitude) * sin(m_Longitude);
		localDir.y = cos(m_Latitude) * cos(m_Longitude);
		localDir.z = sin(m_Latitude);

		
	}
	
	void Zoom(int direction);
private:
	float radians(float degrees)
	{
		return degrees * M_PIf / 180.0f;
	}

	float degrees(float radians)
	{
		return radians * M_1_PIf * 180.0f;
	}
	void UpdateCamera();
	void MoveForward(float speed);
	void MoveLeft(float speed);
	void MoveRight(float speed);
	void MoveUp(float speed);
	void MoveDown(float speed);
	void RollLeft(float speed);
	void RollRight(float speed);
private:
	bool 		m_GimbalLock = false;
	ViewMode 	m_ViewMode = ViewMode::LookAtFixed;
	float 		m_CameraEyeLookatDist = 0.0f;
	float 		m_ZoomMultiplier = 1.1f;
	float 		m_MoveSpeed = 1.0f;
	float 		m_RollSpeed = 0.5f;
	float		m_Latitude = 0.0f; ///< radians
	float		m_Longitude = 0.0f; ///< radians
	int 		m_PrevPosX = 0;
	int 		m_PrevPosY = 0;
	bool 		m_PerformTracking = false;
	optix::float3 U;
	optix::float3 V;
	optix::float3 W;
};	///	!class Trackball
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_UTILS_TRACKBALL_H__