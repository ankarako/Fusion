#include <ScalingNode.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace fu {
namespace trans {
///	\struct Impl
///	\brief ScalingNodeObj implementation
struct ScalingNodeObj::Impl
{
	/// the current native frame
	native_frame_t	m_CurrenNativeFrame;
	///	our current frame
	frame_t			m_CurrentFrame;
	/// the scaling size of the input frame
	uint2 m_ScalingSize{ 0, 0 };
	///	frame flow in
	rxcpp::subjects::subject<native_frame_t> 	m_NativeFrameFlowInSubj;
	/// frame flow output
	rxcpp::subjects::subject<frame_t> 			m_FrameFlowOutSubj;
	///	native frame flow out
	rxcpp::subjects::subject<native_frame_t> 	m_NativeFrameFlowOutSubj;
	///	Construction
	///	\brief sets the native frame flow int task
	Impl() { }
};	///	!struct Impl
///	Construction
///	\brief construct by setting the scaling size of the node
ScalingNodeObj::ScalingNodeObj(unsigned int width, unsigned int height)
	: m_Impl(spimpl::make_unique_impl<Impl>())
{
	/// set scaling size
	m_Impl->m_ScalingSize.x = width;
	m_Impl->m_ScalingSize.y = height;
	/// allocate the current native frame
	m_Impl->m_CurrenNativeFrame =
		cv::Mat::zeros(m_Impl->m_ScalingSize.y, m_Impl->m_ScalingSize.x, CV_8UC4);
	///===============================
	///	native frame flow int task
	///===========================
	m_Impl->m_NativeFrameFlowInSubj.get_observable()
		.subscribe([this](native_frame_t& frame)
	{
		/// resize the incoming frame with opencv
		if (m_Impl->m_ScalingSize.x != frame.size().width && m_Impl->m_ScalingSize.y != frame.size().height)
		{
			cv::resize(frame, m_Impl->m_CurrenNativeFrame, cv::Size(m_Impl->m_ScalingSize.x, m_Impl->m_ScalingSize.y), 0.0, 0.0, cv::INTER_LINEAR);
			/// allocate current frame
			m_Impl->m_CurrentFrame = CreateBufferCPU<uchar4>(m_Impl->m_ScalingSize.x * m_Impl->m_ScalingSize.y);
			/// copy native frame to our frame
			std::memcpy(m_Impl->m_CurrentFrame->Data(), m_Impl->m_CurrenNativeFrame.data, m_Impl->m_CurrentFrame->ByteSize());
		}
		else
		{
			/// allocate current frame
			m_Impl->m_CurrentFrame = CreateBufferCPU<uchar4>(m_Impl->m_ScalingSize.x * m_Impl->m_ScalingSize.y);
			/// copy native frame to our frame
			std::memcpy(m_Impl->m_CurrentFrame->Data(), frame.data, m_Impl->m_CurrentFrame->ByteSize());
		}
		
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_CurrentFrame);
		m_Impl->m_NativeFrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_CurrenNativeFrame);
	});
}
/// \brief set scaling size
///	\param	size	the size of the output frame
void ScalingNodeObj::SetScalingSize(uint2 size)
{
	m_Impl->m_ScalingSize = size;
}
///	\brief set scaling width
///	\param	width	the width to scale the output frame
void ScalingNodeObj::SetScalingWidth(unsigned int width)
{
	m_Impl->m_ScalingSize.x = width;
}
///	\brief set scaling height
///	\param	height	the height to scale the output frame
void ScalingNodeObj::SetScalingHeight(unsigned int height)
{
	m_Impl->m_ScalingSize.y = height;
}
///	\brief native frame input
///	\note supports only native frame input
rxcpp::observer<ScalingNodeObj::native_frame_t> ScalingNodeObj::NativeFrameFlowIn()
{
	return m_Impl->m_NativeFrameFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
///	\brief frame ouput
rxcpp::observable<ScalingNodeObj::frame_t> ScalingNodeObj::FrameFlowOut()
{
	return m_Impl->m_FrameFlowOutSubj.get_observable().as_dynamic();
}
///	\brief native frame flow output
rxcpp::observable<ScalingNodeObj::native_frame_t> ScalingNodeObj::NativeFrameFlowOut()
{
	return m_Impl->m_NativeFrameFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu