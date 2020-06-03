#ifndef __OPTIXRAYTRACER_PUBLIC_SYSTEMS_ANIMATIONSYSTEM_H__
#define __OPTIXRAYTRACER_PUBLIC_SYSTEMS_ANIMATIONSYSTEM_H__

#include <Components/TriangleMeshComp.h>
#include <PerfcapAnimationData.h>
#include <MeshData.h>
#include <array>
#include <optix_world.h>

namespace fu {
namespace rt {

class AnimationSystem
{
public:
	static void GetRTEulerCenter(const optix::float3& trans, const optix::float3& euler, const optix::float3& cRot, optix::Matrix4x4& outMat)
	{
		outMat = optix::Matrix4x4::identity();
		float transx = trans.x;
		float transy = trans.y;
		float transz = trans.z;
		optix::Matrix4x4 heading = GetHeadingMat(euler.y);
		optix::Matrix4x4 pitch = GetPitchMat(euler.x);
		optix::Matrix4x4 bank = GetBankMat(euler.z);
		optix::Matrix4x4 out = heading * pitch * bank;
		out[3] =	cRot.x - (out[0]	* cRot.x + out[1]	* cRot.y + out[2]	* cRot.z) + transx;
		out[7] =	cRot.y - (out[4]	* cRot.x + out[5]	* cRot.y + out[6]	* cRot.z) + transy;
		out[11] =	cRot.z - (out[8]	* cRot.x + out[9]	* cRot.y + out[10] * cRot.z) + transz;
		outMat = out;
		//{
		//	out[0], out[1], out[2], out[3],
		//	out[4], out[5], out[6], out[7],
		//	out[8], out[9], out[10], out[11],
		//	out[12], out[13], out[14], out[15],
		//};
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

	static optix::float3 GetExpMap(const optix::float3& swing1, const optix::float3& swing2, const optix::float3 rotation)
	{
		optix::float3 out;
		out.x = radians(rotation.x) * swing1.x + radians(rotation.y) * swing2.x;
		out.y = radians(rotation.x) * swing1.y + radians(rotation.y) * swing2.y;
		out.z = radians(rotation.x) * swing1.z + radians(rotation.y) * swing2.z;
		return out;
	}

	static optix::float4 ExpMapToQuaternion(const optix::float3& expMap)
	{
		optix::float4 out;
		float theta = optix::length(expMap);
		float s = 0.5f * sinc(theta * 0.5f);
		out.w = std::cosf(theta * 0.5f);
		out.x = s * expMap.x;
		out.y = s * expMap.y;
		out.z = s * expMap.z;
		return out;
	}

	static optix::float4 QuaternionFromAxisAngle(const optix::float3& axis, float angle)
	{
		optix::float4 out;
		float rad = radians(angle);
		float axisNorm = optix::length(axis);
		if (axisNorm < 0.00001f)
		{
			out.x = 0.0f;
			out.y = 0.0f;
			out.z = 0.0f;
			out.w = 1.0f;
		}
		else
		{
			float s = std::sinf(rad * 0.5f) / axisNorm;
			out.w = std::cosf(rad * 0.5f);
			out.x = s * axis.x;
			out.y = s * axis.y;
			out.z = s * axis.z;
		}
		return out;
	}

	static optix::Matrix4x4 TransformFromQuatCRot(const optix::float4& q, const optix::float3& center)
	{
		optix::float4 qNormed = NormalizeQuaternion(q);
		const float w = qNormed.w;
		const float x = qNormed.x;
		const float y = qNormed.y;
		const float z = qNormed.z;
		const float ww = w * w;
		const float xx = x * x;
		const float yy = y * y;
		const float zz = z * z;
		const float xy = x * y;
		const float zw = z * w;
		const float xz = x * z;
		const float yw = y * w;
		const float yz = y * z;
		const float xw = x * w;
		float mat_data[16] = {
			xx - yy - zz + ww,	2.0f * (xy + zw)  ,	2.0f * (xz - yw)  , 0.0f,
			2.0f * (xy - zw) ,	-xx + yy - zz + ww, 2.0f * (yz + xw)  , 0.0f,
			2.0f * (xz + yw) ,	2.0f * (yz - xw)  , -xx - yy + zz + ww, 0.0f,
			0.0f,				0.0f,				0.0f,				1.0f
		};
		mat_data[3] = center.x - center.x * mat_data[0] - center.y * mat_data[1] - center.z * mat_data[2];
		mat_data[7] = center.y - center.x * mat_data[4] - center.y * mat_data[5] - center.z * mat_data[6];
		mat_data[11] = center.z - center.x * mat_data[8] - center.y * mat_data[9] - center.z * mat_data[10];
		optix::Matrix4x4 out = optix::Matrix4x4(mat_data);
		return out;
	}

	static optix::Matrix4x4 GetJointTransform(const io::perfcap_joint_ptr_t& joint, const io::perfcap_skeleton_ptr_t& skeleton, const float3& rotation)
	{
		optix::float3 swing1 = optix::make_float3(
			joint->Swing1[0],
			joint->Swing1[1],
			joint->Swing1[2]
		);
		optix::float3 swing2 = optix::make_float3(
			joint->Swing2[0],
			joint->Swing2[1],
			joint->Swing2[2]
		);
		optix::float3 twist = optix::make_float3(
			joint->Twist[0],
			joint->Twist[1],
			joint->Twist[2]
		);
		optix::float3 expMap = GetExpMap(swing1, swing2, rotation);
		optix::float4 q = ExpMapToQuaternion(expMap);
		int parentId = joint->ParentId;
		optix::float3 initBoneDir = optix::make_float3(joint->Twist[0], joint->Twist[1], joint->Twist[2]);
		optix::float4 initBoneQuaternion = NormalizeQuaternion(optix::make_float4(initBoneDir, 0.0f));
		optix::float4 qConj = optix::make_float4(-q.x, -q.y, -q.z, q.w);
		optix::float4 qMultConj = MultQuaternions(initBoneQuaternion, qConj);
		optix::float4 boneQuaternion = NormalizeQuaternion(MultQuaternions(qMultConj, q));
		optix::float3 newTwist = optix::make_float3(boneQuaternion.x, boneQuaternion.y, boneQuaternion.z);

		optix::float4 q2 = QuaternionFromAxisAngle(newTwist, rotation.z);
		optix::float4 qF = MultQuaternions(q, q2);
		optix::float3 pos = optix::make_float3(
			skeleton->at(joint->ParentId)->Position[0],
			skeleton->at(joint->ParentId)->Position[1],
			skeleton->at(joint->ParentId)->Position[2]
		);
		optix::Matrix4x4 out = TransformFromQuatCRot(qF, pos);
		return out;
	}

	static io::MeshData AnimateMesh(const io::MeshData& mesh, const io::perfcap_skeleton_ptr_t& skeleton, const io::perfcap_skin_data_ptr_t& skin_data, const BufferCPU<float>& animation_params)
	{
		int numVerts = mesh->VertexBuffer->Count();
		int numFaces = mesh->TIndexBuffer->Count();
		int numJoints = skeleton->size();
		/// output mesh data
		io::MeshData outMesh = io::CreateMeshData();
		std::vector<optix::Matrix4x4> localtransforms;
		/// tediousness to decouple root translation and joint rotations
		optix::float3 rootTranslation = optix::make_float3(animation_params->Data()[0], animation_params->Data()[1], animation_params->Data()[2]);
		std::vector<optix::float3> jointRotations;
		jointRotations.emplace_back(optix::make_float3(
			animation_params->Data()[1 * 3 + 0],
			animation_params->Data()[1 * 3 + 1],
			animation_params->Data()[1 * 3 + 2]
		));
		for (int j = 1; j < numJoints; ++j)
		{
			jointRotations.emplace_back(
				optix::make_float3(
					animation_params->Data()[(j + 1) * 3 + 0],
					animation_params->Data()[(j + 1) * 3 + 1],
					animation_params->Data()[(j + 1) * 3 + 2]
				)
			);
		}
		/// create transforms
		for (int j = 0; j < numJoints; ++j)
		{
			io::perfcap_joint_ptr_t joint = skeleton->at(j);
			if (joint->Id == 0)
			{
				optix::float3 rootRot = jointRotations[j];	
				optix::float3 rootPos = optix::make_float3(
					joint->Position[0],
					joint->Position[1],
					joint->Position[2]
				);
				optix::Matrix4x4 rootTrans;
				GetRTEulerCenter(rootTranslation, rootRot, rootPos, rootTrans);
				localtransforms.emplace_back(rootTrans);
			}
			else
			{
				//int jj = j - 1;
				optix::float3 jointRot = jointRotations[j];
				localtransforms.emplace_back(GetJointTransform(joint, skeleton, jointRot));
			}
		}
		std::vector<optix::Matrix4x4> combineTransforms;
		/// setup combine transforms
		for (int j = 0; j < numJoints; ++j)
		{
			io::perfcap_joint_ptr_t joint = skeleton->at(j);
			int numHier = joint->Hierarchy.size();
			const std::vector<int> pathToHier = joint->Hierarchy;
			optix::Matrix4x4 local = localtransforms[j];
			if (numHier > 0)
			{	
				for (int h = 0; h < numHier; ++h)
				{
					optix::Matrix4x4 parentLocal = localtransforms[pathToHier[h]];
					local = parentLocal * local;
				}
				combineTransforms.emplace_back(local);
			}
			else
			{
				combineTransforms.emplace_back(local);
			}
		}
		/// Transform Vertices;
		outMesh->VertexBuffer = CreateBufferCPU<float3>(numVerts);
		outMesh->NormalBuffer = CreateBufferCPU<float3>(numVerts);
		int numWeightsPerVertex = skin_data->NumWeightsPerVertex;
		for (int v = 0; v < numVerts; ++v)
		{
			optix::float4 transVertexPosHomo;
			optix::float4 transNormalHomo;
			optix::float4 inputVertexPosHomo;
			optix::float4 inputNormalHomo;

			optix::float3 outVertexPos;
			optix::float3 outNormal;

			optix::float3 newVertexPos = optix::make_float3(0.0f, 0.0f, 0.0f);
			optix::float3 newNormal = optix::make_float3(0.0f, 0.0f, 0.0f);

			optix::float3 templateVertexPos = mesh->VertexBuffer->Data()[v];
			optix::float3 templateNormal = mesh->NormalBuffer->Data()[v];
			inputVertexPosHomo = optix::make_float4(templateVertexPos.x, templateVertexPos.y, templateVertexPos.z, 1.0f);
			inputNormalHomo = optix::make_float4(templateNormal.x, templateNormal.y, templateNormal.z, 0.0f);

			const float* weightsAtCurrentVertex = &skin_data->WeightData[v * numWeightsPerVertex + 0];
			const int* jointIndicesAtCurrentVertex = &skin_data->Jointdata[v * numWeightsPerVertex + 0];

			for (int s = 0; s < numWeightsPerVertex; ++s)
			{
				float weight = weightsAtCurrentVertex[s];
				float jointId = jointIndicesAtCurrentVertex[s];

				transVertexPosHomo = combineTransforms[jointId] * inputVertexPosHomo;
				transNormalHomo = combineTransforms[jointId] * inputNormalHomo;

				newVertexPos.x += weight * transVertexPosHomo.x / transVertexPosHomo.w;
				newVertexPos.y += weight * transVertexPosHomo.y / transVertexPosHomo.w;
				newVertexPos.z += weight * transVertexPosHomo.z / transVertexPosHomo.w;

				newNormal.x += weight * transNormalHomo.x / transNormalHomo.w;
				newNormal.y += weight * transNormalHomo.y / transNormalHomo.w;
				newNormal.z += weight * transNormalHomo.z / transNormalHomo.w;
			}
			newNormal = optix::normalize(newNormal);
			outMesh->VertexBuffer->Data()[v] = newVertexPos;
			outMesh->NormalBuffer->Data()[v] = newNormal;
		}
		outMesh->TexcoordBuffer = CreateBufferCPU<float2>(mesh->TexcoordBuffer->Count());
		std::memcpy(outMesh->TexcoordBuffer->Data(), mesh->TexcoordBuffer->Data(), mesh->TexcoordBuffer->ByteSize());
		outMesh->TIndexBuffer = CreateBufferCPU<uint3>(mesh->TIndexBuffer->Count());
		std::memcpy(outMesh->TIndexBuffer->Data(), mesh->TIndexBuffer->Data(), mesh->TIndexBuffer->ByteSize());
		return outMesh;
	}
private:
	static inline float sinc(float x)
	{
		if (std::fabsf(x) < 0.32f)
		{
			return 1.0f - x * x / 6.0f;
		}
		else
		{
			return std::sinf(x) / x;
		}
	}

	static float radians(float degrees)
	{
		return degrees / 180.f * M_PIf;
	}

	static optix::float4 MultQuaternions(const optix::float4& q1, const optix::float4& q2)
	{
		optix::float4 res;
		res.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
		res.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
		res.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
		res.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
		return res;
	}

	static float QuaternionNorm(const optix::float4& q)
	{
		return std::sqrtf(
			q.w * q.w +
			q.x * q.x +
			q.y * q.y +
			q.z * q.z);
	}

	static optix::float4 NormalizeQuaternion(const optix::float4& q)
	{
		float norm = QuaternionNorm(q);
		return optix::make_float4(
			q.x / norm,
			q.y / norm,
			q.z / norm,
			q.w / norm);
	}
};
}	///	!namespace rt
}	///	!namespace fu

#endif	///	!__OPTIXRAYTRACER_PUBLIC_SYSTEMS_ANIMATIONSYSTEM_H__