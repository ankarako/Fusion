#ifndef	__FUSION_PUBLIC_MODELS_PERFCAPPLAYERMODEL_H__
#define __FUSION_PUBLIC_MODELS_PERFCAPPLAYERMODEL_H__

#include <MeshData.h>
#include <PerfcapAnimationData.h>
#include <PerfcapTrackedData.h>
#include <TrackedData.h>
#include <Core/SequenceItem.h>
#include <string>
#include <rxcpp/rx.hpp>
#include <spimpl.h>
#include <tuple>

namespace fu {
namespace fusion {

class PerfcapPlayerModel
{
public:
	using perfcap_tex_mesh_t = std::tuple<io::MeshData, io::perfcap_skeleton_ptr_t, io::perfcap_skin_data_ptr_t, io::tracked_seq_ptr_t, std::string, std::string>;
	using template_mesh_t = std::tuple<io::MeshData, BufferCPU<uchar4>, uint2>;
	/// Construction
	PerfcapPlayerModel();

	void Start();
	void Pause();
	void Stop();
	void SeekFrame(size_t frameId);
	void SeekForward();
	void SeekBackward();
	/// 
	rxcpp::observable<SequenceItem>				SequenceItemFlowOut();
	rxcpp::observable<template_mesh_t>			TemplateMeshDataFlowOut();
	rxcpp::observable<io::MeshData>				AnimatedMeshDataFlowOut();
	rxcpp::observable<void*>					AnimationFrameCompleted();
	rxcpp::observer<perfcap_tex_mesh_t>			PerfcapMeshDataFlowIn();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PerfcapPlayerModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_PERFCAPPLAYERMODEL_H__