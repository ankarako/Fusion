#include <Models/MultiViewPlayerModel.h>
#include <DecodingNode.h>
#include <filesystem>

namespace fu {
namespace mvt {
/// \struct Impl
///	\brief MultiViewPlayerModel Implementation
struct MultiViewPlayerModel::Impl
{
	std::vector<trans::DecodingNode>	m_DecodingNodes;
	std::vector<BufferCPU<uchar4>>		m_CurrentFrames;
	std::string							m_DebugOutputDir;
	bool m_DebugFramesEnabled{ false };
	bool m_UndistortEnabled{ false };
	rxcpp::subjects::subject<std::vector<DistCoeffs>>			m_DistortionCoefficientsFlowInSubj;
	rxcpp::subjects::subject<std::vector<BufferCPU<float>>>		m_CameraMatricesFlowInSubj;
	rxcpp::subjects::subject<std::vector<std::string>>			m_VideoFilepathsFlowInSubj;
	rxcpp::subjects::subject<std::vector<BufferCPU<uchar4>>>	m_MultiViewFramesFlowOutSubj;
	rxcpp::subjects::subject<int>								m_SeekframeFlowInSubj;
	/// Construction
	Impl () { }
};	///	!struct Impl
/// Construction
MultiViewPlayerModel::MultiViewPlayerModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
/// \brief model initialization
void MultiViewPlayerModel::Init()
{
	///==============================
	/// Video Filepaths Flow in Task
	///==============================
	m_Impl->m_VideoFilepathsFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::vector<std::string>& filepaths) 
	{
		for (const std::string& filepath : filepaths)
		{
			trans::DecodingNode node = trans::CreateDecodingNode();
			node->LoadFile(filepath);
			node->SetUndistortEnabled(m_Impl->m_UndistortEnabled);
			node->SetSaveDebugFramesEnabled(m_Impl->m_DebugFramesEnabled);
			node->SetDebugFramesOutDir(m_Impl->m_DebugOutputDir);
			m_Impl->m_DecodingNodes.emplace_back(node);
		}
		
	});
	///=============
	/// Seek frame
	///=============
	m_Impl->m_SeekframeFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](int frame) 
	{
		std::vector<BufferCPU<uchar4>> frames;
		for (int n = 0; n < m_Impl->m_DecodingNodes.size(); n++)
		{
			frames.emplace_back(m_Impl->m_DecodingNodes[n]->GetFrame(frame));
		}
		m_Impl->m_MultiViewFramesFlowOutSubj.get_subscriber().on_next(frames);
	});
	///=========================
	/// Camera Matrices Flow in
	///=========================
	m_Impl->m_CameraMatricesFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::vector<BufferCPU<float>>& mats) 
	{
		int count = mats.size();
		for (int c = 0; c < count; ++c)
		{
			m_Impl->m_DecodingNodes[c]->SetCameraMatrix(mats[c]);
		}
	
	});
	///================================
	/// Distortion Coefficients flow in
	///================================
	m_Impl->m_DistortionCoefficientsFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::vector<DistCoeffs>& coeffs) 
	{
		int count = coeffs.size();
		for (int c = 0; c < count; ++c)
		{
			m_Impl->m_DecodingNodes[c]->SetDisrtionCoefficients(coeffs[c]);
		}
	});
}
/// \brief model destruction
void MultiViewPlayerModel::Destroy()
{
	for (trans::DecodingNode& node : m_Impl->m_DecodingNodes)
	{
		node->Release();
	}
}

void MultiViewPlayerModel::SetUndistortEnabled(bool enabled)
{
	m_Impl->m_UndistortEnabled = enabled;
}

void MultiViewPlayerModel::SetDebugFramesEnabled(bool enabled)
{
	m_Impl->m_DebugFramesEnabled = enabled;
}

void MultiViewPlayerModel::SetDebugOutDir(const std::string & outdir)
{
	using namespace std::experimental;
	m_Impl->m_DebugOutputDir = outdir;
	if (!filesystem::exists(outdir))
	{
		filesystem::create_directory(outdir);
	}
}

const std::string& MultiViewPlayerModel::GetDebugOutDir() const
{
	return m_Impl->m_DebugOutputDir;
}


rxcpp::observer<std::vector<BufferCPU<float>>> MultiViewPlayerModel::CameraMatricesFlowIn()
{
	return m_Impl->m_CameraMatricesFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<std::vector<DistCoeffs>> MultiViewPlayerModel::DistortionCoefficientsFlowIn()
{
	return m_Impl->m_DistortionCoefficientsFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<std::vector<std::string>> MultiViewPlayerModel::VideoFilepathsFlowIn()
{
	return m_Impl->m_VideoFilepathsFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<int> MultiViewPlayerModel::SeekFrameIdFlowIn()
{
	return m_Impl->m_SeekframeFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observable<std::vector<BufferCPU<uchar4>>> MultiViewPlayerModel::MultiViewFramesFlowOut()
{
	return m_Impl->m_MultiViewFramesFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace mvt
}	///	!namespace fu