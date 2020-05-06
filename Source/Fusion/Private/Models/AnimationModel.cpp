#include <Models/AnimationModel.h>
#include <plog/Log.h>
#include <Systems/AnimationSystem.h>

namespace fu {
namespace fusion {

struct AnimationModel::Impl
{
	io::TemplateMesh	m_TemplateMesh;

	rxcpp::subjects::subject<io::AnimationFrame>		m_AnimationFrameFlowInSubj;
	rxcpp::subjects::subject<io::TemplateMesh>			m_TemplateMeshFlowInSubj;
	rxcpp::subjects::subject<io::MeshData>				m_MeshDataFlowOutSubj;
	Impl() { }


};	///	!struct Impl
/// Construction
AnimationModel::AnimationModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }

void AnimationModel::Init()
{
	///=========================
	/// Template mesh flow in
	///========================
	m_Impl->m_TemplateMeshFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const io::TemplateMesh& mesh) 
	{
		m_Impl->m_TemplateMesh = mesh;
	});
	///==========================
	/// Animation Frame Flow In
	///===========================
	m_Impl->m_AnimationFrameFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const io::AnimationFrame& frame) 
	{
		/// Animate!
		LOG_DEBUG << "I got frame for animatio";
		/// =====================
		/// Get frame parameters
		///=======================
		std::array<float, 3> rootTranslation =
		{
			frame.Skeleton.GlobalRootTranslation.x,
			frame.Skeleton.GlobalRootTranslation.y,
			frame.Skeleton.GlobalRootTranslation.z
		};

		std::array<float, 3> rootRot =
		{
			frame.Skeleton.JointRotations[0].x,
			frame.Skeleton.JointRotations[0].y,
			frame.Skeleton.JointRotations[0].z
		};
		std::vector<std::array<float, 3>> rotations;
		auto nodeCount = frame.Skeleton.JointRotations.size();
		for (int j = 1; j < nodeCount; j++)
		{
			std::array<float, 3> rot =
			{
				frame.Skeleton.JointRotations[j].x,
				frame.Skeleton.JointRotations[j].y,
				frame.Skeleton.JointRotations[j].z
			};
			rotations.emplace_back(rot);
		}
		///================
		/// Get transforms
		///================
		std::vector<std::array<float, 16>> JointTransforms;
		for (int j = 0; j < nodeCount; j++)
		{
			if (j == 0)
			{
				std::array<float, 16> rootTrans;
				std::array<float, 3> rootInitPos =
				{
					m_Impl->m_TemplateMesh.Skeleton[0].Position.x,
					m_Impl->m_TemplateMesh.Skeleton[0].Position.y,
					m_Impl->m_TemplateMesh.Skeleton[0].Position.z
				};
				rt::AnimationSystem::GetRTEulerCenter(rootTranslation, rootRot, rootInitPos, rootTrans);
				JointTransforms.emplace_back(rootTrans);
			}
			else
			{

			}
		}
	});
}

rxcpp::observer<io::AnimationFrame> fu::fusion::AnimationModel::AnimationFrameFlowIn()
{
	return m_Impl->m_AnimationFrameFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<io::TemplateMesh> fu::fusion::AnimationModel::TemplateMeshFlowIn()
{
	return m_Impl->m_TemplateMeshFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<io::MeshData> fu::fusion::AnimationModel::MeshDataFlowOut()
{
	return m_Impl->m_MeshDataFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu