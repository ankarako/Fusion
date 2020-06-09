#include <Models/PerfcapPlayerModel.h>
#include <Core/Helpers.h>
#include <DecodingNode.h>
#include <Systems/AnimationSystem.h>
#include <plog/Log.h>
#include <filesystem>
#include <chrono>

#ifdef _MSC_VER
#undef GetObject
#endif

namespace fu {
namespace fusion {

struct PerfcapPlayerModel::Impl
{	
	io::MeshData				m_TemplateMesh;
	io::perfcap_skeleton_ptr_t	m_Skeleton;
	io::perfcap_skin_data_ptr_t	m_SkinData;
	io::tracked_seq_ptr_t		m_TrackedParams;
	trans::DecodingNode			m_TextureDecNode;

	int m_CurrentFrame{ 0 };

	rxcpp::subjects::subject<perfcap_tex_mesh_t>	m_PerfcapMeshDataFlowInSubj;
	rxcpp::subjects::subject<SequenceItem>			m_SequenceItemFlowOutSubj;
	rxcpp::subjects::subject<template_mesh_t>		m_TemplateMeshFlowOutSubj;
	rxcpp::subjects::subject<io::MeshData>			m_AnimatedMeshDataFlowOutSubj;

	rxcpp::composite_subscription						m_PlaybackLifetime;
	rxcpp::observable<long>								m_PlaybackObservable;
	std::chrono::milliseconds							m_FramePeriod;
	/// Construction
	Impl() 
		: m_TextureDecNode(trans::CreateDecodingNode())
	{ }
};	///	!struct Impl
/// Construction
PerfcapPlayerModel::PerfcapPlayerModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ 
	///===========================
	/// Perfcap mesh data flow in
	///===========================
	m_Impl->m_PerfcapMeshDataFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const perfcap_tex_mesh_t& mesh) 
	{
		m_Impl->m_TemplateMesh = std::get<0>(mesh);
		m_Impl->m_Skeleton = std::get<1>(mesh);
		m_Impl->m_SkinData = std::get<2>(mesh);
		m_Impl->m_TrackedParams = std::get<3>(mesh);
		std::string texFPath = std::get<4>(mesh);
		m_Impl->m_TextureDecNode->LoadFile(texFPath);
		std::string name = std::get<5>(mesh);

		/// make a sequence item
		SequenceItem item;
		item.Name = name;
		item.Expanded = false;
		item.FrameStart = 0;
		item.FrameEnd = m_Impl->m_TrackedParams->size();
		item.Duration = m_Impl->m_TrackedParams->size();
		item.SeqFrameStart = 0;
		item.SeqFrameEnd = m_Impl->m_TrackedParams->size();
		item.Type = SequenceItemType::Animation;
		m_Impl->m_SequenceItemFlowOutSubj.get_subscriber().on_next(item);
		/// send template mesh

		/// get the first texture frame
		BufferCPU<uchar4> texFrame = m_Impl->m_TextureDecNode->GetFrame(0);
		uint2 reso = make_uint2(m_Impl->m_TextureDecNode->GetFrameWidth(), m_Impl->m_TextureDecNode->GetFrameHeight());
		auto texturedOutput = std::make_tuple(m_Impl->m_TemplateMesh, texFrame, reso);
		m_Impl->m_TemplateMeshFlowOutSubj.get_subscriber().on_next(texturedOutput);
	});
	

	double fps = 25.0;
	double periodSecs = 1.0 / fps;
	m_Impl->m_FramePeriod =
		std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(periodSecs));

	std::chrono::nanoseconds periodNano =
		std::chrono::duration_cast<std::chrono::nanoseconds>(m_Impl->m_FramePeriod);

	m_Impl->m_PlaybackObservable = rxcpp::observable<>::interval(periodNano).as_dynamic();
}


void PerfcapPlayerModel::Start()
{
	m_Impl->m_PlaybackLifetime.add(
		m_Impl->m_PlaybackObservable.subscribe(
			[this](auto _)
	{
		size_t dur = m_Impl->m_TrackedParams->size();
		if (m_Impl->m_CurrentFrame < dur)
		{
			
			io::TrackedParams trackedFrame = m_Impl->m_TrackedParams->at(m_Impl->m_CurrentFrame);
			io::MeshData animatedData = rt::AnimationSystem::AnimateMesh(m_Impl->m_TemplateMesh, m_Impl->m_Skeleton, m_Impl->m_SkinData, trackedFrame);
			m_Impl->m_AnimatedMeshDataFlowOutSubj.get_subscriber().on_next(animatedData);
			m_Impl->m_CurrentFrame++;
		}
	}));
}

void PerfcapPlayerModel::Pause()
{
	m_Impl->m_PlaybackLifetime.clear();
}

void PerfcapPlayerModel::Stop()
{
	m_Impl->m_PlaybackLifetime.clear();
	SeekFrame(0);
}

void PerfcapPlayerModel::SeekFrame(size_t frameId)
{
	size_t dur = m_Impl->m_TrackedParams->size();
	if (frameId < dur)
	{

		io::TrackedParams trackedFrame = m_Impl->m_TrackedParams->at(frameId);
		io::MeshData animatedData = rt::AnimationSystem::AnimateMesh(m_Impl->m_TemplateMesh, m_Impl->m_Skeleton, m_Impl->m_SkinData, trackedFrame);
		m_Impl->m_AnimatedMeshDataFlowOutSubj.get_subscriber().on_next(animatedData);
		m_Impl->m_CurrentFrame = frameId;
	}
}

void PerfcapPlayerModel::SeekForward()
{

}

void PerfcapPlayerModel::SeekBackward()
{

}

rxcpp::observable<SequenceItem> fu::fusion::PerfcapPlayerModel::SequenceItemFlowOut()
{
	return m_Impl->m_SequenceItemFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<PerfcapPlayerModel::template_mesh_t> PerfcapPlayerModel::TemplateMeshDataFlowOut()
{
	return m_Impl->m_TemplateMeshFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<io::MeshData> PerfcapPlayerModel::AnimatedMeshDataFlowOut()
{
	return m_Impl->m_AnimatedMeshDataFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observer<PerfcapPlayerModel::perfcap_tex_mesh_t> PerfcapPlayerModel::PerfcapMeshDataFlowIn()
{
	return m_Impl->m_PerfcapMeshDataFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu