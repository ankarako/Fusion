#ifndef __OPTIXRAYTRACER_PUBLIC_SYSTEMS_ANIMATIONSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_ANIMATIONSYSTEM_H__

#include <Components/TriangleMeshComp.h>
#include <Perfcap/AnimationFrame.h>
#include <array>
#include <optix_world.h>

namespace fu {
namespace rt {

class AnimationSystem
{
public:
	static void AnimateTriangleMeshComp(TriangleMeshComp& trComp, const io::AnimationFrame& frame, const std::vector<std::vector<float>>& skin)
	{
		/// copy the vertex buffer
		
	}

	static void GetRTEulerCenter(const std::array<float, 3>& trans, const std::array<float, 3>& euler, const std::array<float, 3>& cRot, std::array<float, 16>& outMat)
	{
		outMat =
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		float transx = trans[0] / 1000.0f;
		float transy = trans[1] / 1000.0f;
		float transz = trans[2] / 1000.0f;
		optix::Matrix4x4 heading = GetHeadingMat(euler[1]);
		optix::Matrix4x4 pitch = GetPitchMat(euler[0]);
		optix::Matrix4x4 bank = GetBankMat(euler[3]);
		optix::Matrix4x4 out = heading * pitch * bank;
		out[3] =	cRot[0] - (out[0]	* cRot[0] + out[1]		* cRot[1] + out[2]	* cRot[2]) + transx;
		out[7] =	cRot[1] - (out[4]	* cRot[0] + out[5]		* cRot[1] + out[6]	* cRot[2]) + transy;
		out[11] =	cRot[2] - (out[8]	* cRot[0] + out[9]	* cRot[1] + out[10] * cRot[2]) + transz;
		outMat =
		{
			out[0], out[1], out[2], out[3],
			out[4], out[5], out[6], out[7],
			out[8], out[9], out[10], out[11],
			out[12], out[13], out[14], out[15],
		};
	}
	///	\brief get the heading matrix from a specified angle
	///	\param	angle	the rotation angle
	///	\return the heading matrix
	static optix::Matrix4x4 GetHeadingMat(float angle)
	{
		float rad = angle * M_PIf / 180.0f;
		std::array<float, 16> outMat =
		{
			std::cosf(rad),  0.0f, std::sinf(rad),	0.0f,
			0.0f,			 1.0f, 0.0f,			0.0f,
			-std::sinf(rad), 0.0f, std::cosf(rad),	0.0f,
			0.0f,			 0.0f, 0.0f,			1.0f
		};
		return optix::Matrix4x4(outMat.data());
	}
	///	\brief get the pitch matrix of the specified angle
	///	\param	angle	the rotation angle
	///	\return the heading matrix
	static optix::Matrix4x4 GetPitchMat(float angle)
	{
		float rad = angle * M_PIf / 180.0f;
		std::array<float, 16> outMat =
		{
			1.0f, 0.0f,	0.0f, 0.0f,
			0.0f, std::cosf(rad), -std::sinf(rad), 0.0f,
			0.0f, std::sinf(rad), std::cosf(rad), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		return optix::Matrix4x4(outMat.data());
	}
	///	\brief get the bank matrix of a specified angle
	///	\param	angle the rotation angle
	///	\return the bank matrix
	static optix::Matrix4x4 GetBankMat(float angle)
	{
		float rad = angle * M_PIf / 180.0f;
		std::array<float, 16> outMat = {
			std::cosf(rad), -std::sinf(rad), 0.0f, 0.0f,
			std::sinf(rad), std::cosf(rad), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		return optix::Matrix4x4(outMat.data());
	}
};
}	///	!namespace rt
}	///	!namespace fu
#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_ANIMATIONSYSTEM_H__