#ifndef __VIDEOTRANSCODER_PUBLIC_DECODINGNODE_H__
#define __VIDEOTRANSCODER_PUBLIC_DECODINGNODE_H__

#include <Buffer.h>
#include <spimpl.h>
#include <memory>
#include <string>
#include <rxcpp/rx.hpp>
#include <DistortionCoefficients.h>
#include <opencv2/core.hpp>

namespace fu {
namespace trans {
///	\struct DecodingNodeObj
///	\brief a node that is able to decode video files
///	It demuxes the video stream (only) from a video file and outputs
///	the decoded video frames
class DecodingNodeObj
{
public:
	///	\typedef frame_t
	///	\brief the frame object type
	///	a decoding node's frame type is the same as the underlying librarie's type (for now)
	using frame_t = BufferCPU<uchar4>;
	///	\typedef native_frame_t
	///	\brief the native frame type (the type of the decoding library)
	using native_frame_t = cv::Mat;
	/// Construction
	///	\brief Default constructor
	///	does nothing. The decoding node initializes its members
	///	from oening files
	DecodingNodeObj();
	///	\brief load a video file
	///	\param	filepath the path to the file to load
	void LoadFile(const std::string& filepath);
	///	\brief release the underlying decoding context
	void Release();
	///	\brief get the frame rate of the loaded video
	///	\return the frame rate of the loaded video
	double GetFrameRate() const;
	///	\brief get the frame width of the loaded video
	///	\return the frame width of the loaded video
	size_t GetFrameWidth() const;
	///	\brief get the frame height of the loaded video
	///	\return the frame height of the loaded video
	size_t GetFrameHeight() const;
	///	\brief get the current frame Position
	///	\return the current frame position
	size_t GetCurrentFramePosition() const;
	///	\brief get the frame count of the loaded video
	///	\return the frame count of the loaded video
	size_t GetFrameCount() const;
	///	\brief set the decoding node's current frame position
	///	\param	framePos	the frame position to set
	void SetCurrentFramePos(size_t framePos);
	///	\brief generate the frame at the current frame position
	///	Outputs a frame buffer from the node's output
	void GenerateFrame();
	///	\brief get the specified frame
	///	\param	id	the frame id
	///	\return the specified frame
	BufferCPU<uchar4> GetFrame(int id);
	/// \brief rotate the video images
	///	\param 
	void RotateFrames90CW(bool rot);
	/// \brief set distortion coefficients
	void SetCameraMatrix(const BufferCPU<float>& mat);
	void SetDisrtionCoefficients(const DistCoeffs& coeffs);
	void SetUndistortEnabled(bool enabled);
	void SetSaveDebugFramesEnabled(bool enabled);
	///	\brief generate a specific number of frames
	///	generates the specified number of frames
	///	\param	frameCount	the number of frames to generate
	///	\note generates the specified number of frames from the current frame position
	void GenerateFrames(size_t frameCount);
	///	\brief set scaled size
	///	\param	width	the new width of the decoded frame
	///	\param	height	the new height of the decoded frame
	void SetScaledSize(size_t width, size_t height);
	///
	void SetGeneratingFrames(bool val);
	///	\brief frame output
	///	decoding nodes have only output frame streams
	rxcpp::observable<frame_t> FrameFlowOut();
	///	\brief native frame output
	///	native frame output for hooking in other nodes
	rxcpp::observable<native_frame_t> NativeFrameFlowOut();
	///	\brief generate the specified amount of frames
	rxcpp::observer<size_t>		PrefetchFrames();
	///	\brief generate frames task completed notification
	rxcpp::observable<void*>	PrefetchFramesCompleted();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!struct DecodingNodeObj
///	\typedef DecodingNode
///	\brief a reference counting DecodingNodeObj
using DecodingNode = std::shared_ptr<DecodingNodeObj>;
///	\brief create a decopding node
static DecodingNode CreateDecodingNode()
{
	return std::make_shared<DecodingNodeObj>();
}
}	///	!namespace trans
}	///	!namespace fu
#endif	///	!__VIDEOTRANSCODER_PUBLIC_DECODINGNODE_H__