#include <Models/MultiViewPlayerModel.h>
#include <DecodingNode.h>


namespace fu {
namespace mvt {
/// \struct Impl
///	\brief MultiViewPlayerModel Implementation
struct MultiViewPlayerModel::Impl
{
	std::vector<trans::DecodingNode> m_DecodingNodes;
	std::vector<BufferCPU<uchar4>>	m_CurrentFrames;
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
			m_Impl->m_DecodingNodes.emplace_back(node);
		}
	});

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
}
/// \brief model destruction
void MultiViewPlayerModel::Destroy()
{
	for (trans::DecodingNode& node : m_Impl->m_DecodingNodes)
	{
		node->Release();
	}
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