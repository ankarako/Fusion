#ifndef __VIDEOTRANSCODER_PUBLIC_SCALINGNODE_H__
#define __VIDEOTRANSCODER_PUBLIC_SCALINGNODE_H__

#include <Buffer.h>
#include <spimpl.h>
#include <memory>
#include <rxcpp/rx.hpp>

namespace cv {
class Mat;
}	///	!namespace cv

namespace fu {
namespace trans {
///	\class ScalingNodeObj
///	\brief performs scaling on input frames
///	\note supports only bilinear interpolation for now
///	TODO: maybe add mode interpolation nodes
class ScalingNodeObj
{
public:
	///	\typedef frame_t
	///	\brief the output frame type
	using frame_t = BufferCPU<uchar4>;
	///	\typedef native_frame_t
	///	\brief the native frame type (i.e. opencv's frame type)
	using native_frame_t = cv::Mat;
	///	Construction
	///	\brief construct by setting the scaling size of the node
	ScalingNodeObj(unsigned int width, unsigned int height);
	/// \brief set scaling size
	///	\param	size	the size of the output frame
	void SetScalingSize(uint2 size);
	///	\brief set scaling width
	///	\param	width	the width to scale the output frame
	void SetScalingWidth(unsigned int width);
	///	\brief set scaling height
	///	\param	height	the height to scale the output frame
	void SetScalingHeight(unsigned int height);
	///	\brief native frame input
	///	\note supports only native frame input
	rxcpp::observer<native_frame_t> NativeFrameFlowIn();
	///	\brief frame ouput
	rxcpp::observable<frame_t> FrameFlowOut();
	///	\brief native frame flow output
	rxcpp::observable<native_frame_t> NativeFrameFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class ScalingNodeObj
///	\typedef ScalingNode
///	\brief a reference counted scaling node
using ScalingNode = std::shared_ptr<ScalingNodeObj>;
///	\brief create a scaling node
static ScalingNode CreateScalingNode(unsigned int width, unsigned int height)
{
	return std::make_shared<ScalingNodeObj>(width, height);
}
}	///	!namespace trans
}	///	!namespace fu
#endif	///	!__VIDEOTRANSCODER_PUBLIC_SCALINGNODE_H__