#include <DecodingNode.h>
#include <FuAssert.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/calib3d.hpp>
#include <thread>
#include <plog/Log.h>
#include <atomic>
#include <filesystem>


namespace fu {
namespace trans {
///	\struct Impl
///	\brief DecodingNodeObj's implementation
struct DecodingNodeObj::Impl
{
	using dec_t = std::shared_ptr<cv::VideoCapture>;
	/// loaded filepath
	/// TODO: maybe redundant
	std::string m_LoadedFile{ "" };
	std::string m_DebugOutDir{ "" };
	///	current frfame buffer
	frame_t				m_CurrentFrame;
	/// keep the cv::Mat frame too so we don't have 
	///	to allocate new frames for each loaded frame
	cv::Mat				m_CurrentFrameNative;
	///	the current frame position
	size_t				m_CurrentFramePosition{ 0 };
	///	frame width
	size_t		m_FrameWidth{ 0 };
	///	frame height
	size_t		m_FrameHeight{ 0 };
	/// scaled frame width
	size_t		m_ScaledFrameWidth{ 0 };
	/// scaled frame height
	size_t		m_ScaledFrameHeight{ 0 };
	/// frame size in bytes
	size_t		m_FrameByteSize{ 0 };
	///	the video's duration in frames
	size_t		m_FrameCount{ 0 };
	///	frame rate
	double		m_FrameRate{ 0 };

	cv::Mat		m_CameraMatrix;
	cv::Mat		m_OptimalCamMatrix;
	///	the actual opencv video decoder
	dec_t		m_Decoder;
	/// 
	bool		m_Rotate{ false };
	cv::Mat		m_DistCoeffs;
	bool		m_Undistort{ false };
	bool		m_DebugFramesEnabled{ false };
	bool		m_ChromaKeyBGEnabled{ false };
	uchar4		m_ChromaKey{ 0, 255, 0, 255 };
	///
	std::atomic_bool	m_GenerateFrames{ false };
	///	prefetch event task
	rxcpp::subjects::subject<size_t>			m_GenerateFramesTask;
	///	frame output
	rxcpp::subjects::subject<frame_out_t>			m_FrameFlowOutSubj;
	/// native frame output
	rxcpp::subjects::subject<native_frame_t>	m_NativeFrameFlowOutSubj;
	///	finished prefetching notification
	rxcpp::subjects::subject<void*>				m_GenerateFramesTaskCompetedSubj;
	///	Construction
	Impl() 
		: m_Decoder(std::make_shared<cv::VideoCapture>())
	{ }
};	///	!struct Impl
///	 Construction
///	\brief Default constructor
///	does nothing. The decoding node initializes its members
///	from oening files
DecodingNodeObj::DecodingNodeObj()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ 
	///=====================
	///	Generate frames task
	///=====================
	///	param frameCount	the number of frames to generate
	//m_Impl->m_GenerateFramesTask.get_observable().as_dynamic()
	//	.subscribe([this](const size_t frameCount) 
	//{
	//	if (m_Impl->m_Decoder->isOpened() && m_Impl->m_CurrentFramePosition < m_Impl->m_FrameCount)
	//	{
	//		LOG_DEBUG << "Generating on thread: " << std::this_thread::get_id();
	//		unsigned int counter = 0;
	//		while (counter < frameCount && m_Impl->m_GenerateFrames.load(std::memory_order_seq_cst))
	//		{
	//			if (m_Impl->m_Decoder->read(m_Impl->m_CurrentFrameNative))
	//			{
	//				cv::cvtColor(m_Impl->m_CurrentFrameNative, m_Impl->m_CurrentFrameNative, cv::COLOR_BGR2RGBA);
	//				/// get the byte size of the native frame
	//				size_t bsize = m_Impl->m_CurrentFrameNative.total() * m_Impl->m_CurrentFrameNative.elemSize();
	//				size_t fbsize = m_Impl->m_CurrentFrame->ByteSize();
	//				/// check that our frame and the native have the same byte size
	//				DebugAssertMsg(bsize == m_Impl->m_CurrentFrame->ByteSize(), "Decoded native frame type has different byte size.");
	//				/// copy native frame data to our buffer
	//				std::memcpy(m_Impl->m_CurrentFrame->Data(), m_Impl->m_CurrentFrameNative.data, m_Impl->m_FrameByteSize);
	//				/// notify subscriber's about the current frame
	//				m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_CurrentFrame);
	//				m_Impl->m_NativeFrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_CurrentFrameNative);
	//				///	increment counter
	//				counter++;
	//				m_Impl->m_CurrentFramePosition++;
	//				m_Impl->m_Decoder->set(cv::CAP_PROP_POS_FRAMES, m_Impl->m_CurrentFramePosition);
	//			}
	//			else
	//			{
	//				break;
	//			}
	//		}
	//		LOG_DEBUG << "Finished Generating on thread: " << std::this_thread::get_id() << "counter: " << counter;
	//		m_Impl->m_GenerateFramesTaskCompetedSubj.get_subscriber().on_next(nullptr);
	//	}
	//});
}
///	\brief load a video file
///	\param	filepath the path to the file to load
void DecodingNodeObj::LoadFile(const std::string& filepath)
{
	m_Impl->m_Decoder->open(filepath);
	if (m_Impl->m_Decoder->isOpened())
	{
		m_Impl->m_LoadedFile = filepath;
		m_Impl->m_FrameWidth = m_Impl->m_Decoder->get(cv::CAP_PROP_FRAME_WIDTH);
		m_Impl->m_FrameHeight = m_Impl->m_Decoder->get(cv::CAP_PROP_FRAME_HEIGHT);
		m_Impl->m_FrameCount = m_Impl->m_Decoder->get(cv::CAP_PROP_FRAME_COUNT);
		m_Impl->m_FrameRate = m_Impl->m_Decoder->get(cv::CAP_PROP_FPS);
		m_Impl->m_ScaledFrameWidth = m_Impl->m_FrameWidth;
		m_Impl->m_ScaledFrameHeight = m_Impl->m_FrameHeight;
		/// output frame byte size
		m_Impl->m_FrameByteSize = m_Impl->m_FrameWidth * m_Impl->m_FrameHeight * sizeof(uchar4);
		/// initialize the buffer according to the frame's dims
		m_Impl->m_CurrentFrame= CreateBufferCPU<uchar4>(m_Impl->m_FrameHeight * m_Impl->m_FrameWidth);
		m_Impl->m_CurrentFrameNative = cv::Mat::zeros(m_Impl->m_FrameHeight, m_Impl->m_FrameWidth, CV_8UC4);
		///	get the byte size of the native frame
		size_t bsize = m_Impl->m_CurrentFrameNative.total() * m_Impl->m_CurrentFrameNative.elemSize();
		/// check that our frame and the native have the same byte size
		DebugAssertMsg(bsize == m_Impl->m_CurrentFrame->ByteSize(), "Decoded native frame type has different byte size.");
		LOG_DEBUG << "Loaded Video file: " << filepath;
		LOG_DEBUG << "Frame Size       : " << m_Impl->m_FrameWidth << " x " << m_Impl->m_FrameHeight;
		LOG_DEBUG << "Frame Rate       : " << m_Impl->m_FrameRate;
		m_Impl->m_Decoder->set(cv::CAP_PROP_POS_FRAMES, 0.0);
	}
}
///	\brief release the underlying decoding context
void DecodingNodeObj::Release()
{
	if (m_Impl->m_Decoder->isOpened())
		m_Impl->m_Decoder->release();
}
///	\brief get the frame rate of the loaded video
///	\return the frame rate of the loaded video
double DecodingNodeObj::GetFrameRate() const
{
	return m_Impl->m_FrameRate;
}
///	\brief get the frame width of the loaded video
///	\return the frame width of the loaded video
size_t DecodingNodeObj::GetFrameWidth() const
{
	return m_Impl->m_FrameWidth;
}
///	\brief get the frame height of the loaded video
///	\return the frame height of the loaded video
size_t DecodingNodeObj::GetFrameHeight() const
{
	return m_Impl->m_FrameHeight;
}
///	\brief get the current frame Position
///	\return the current frame position
size_t DecodingNodeObj::GetCurrentFramePosition() const
{
	return m_Impl->m_CurrentFramePosition;
}
///	\brief get the frame count of the loaded video
///	\return the frame count of the loaded video
size_t DecodingNodeObj::GetFrameCount() const
{
	return m_Impl->m_FrameCount;
}
///	\brief set the decoding node's current frame position
///	\param	framePos	the frame position to set
void DecodingNodeObj::SetCurrentFramePos(size_t framePos)
{
	if (m_Impl->m_Decoder->isOpened() && framePos < m_Impl->m_FrameCount)
	{
		m_Impl->m_Decoder->set(cv::CAP_PROP_POS_FRAMES, (double)framePos);
		m_Impl->m_CurrentFramePosition = framePos;
	}
}
///	\brief generate the frame at the current frame position
///	Outputs a frame buffer from the node's output
void DecodingNodeObj::GenerateFrame()
{
	if (m_Impl->m_Decoder->isOpened() && m_Impl->m_CurrentFramePosition < m_Impl->m_FrameCount)
	{
		
		m_Impl->m_Decoder->read(m_Impl->m_CurrentFrameNative);
		cv::cvtColor(m_Impl->m_CurrentFrameNative, m_Impl->m_CurrentFrameNative, cv::COLOR_BGR2RGBA);
		/// get the byte size of the native frame
		size_t bsize = m_Impl->m_CurrentFrameNative.total() * m_Impl->m_CurrentFrameNative.elemSize();
		size_t fbsize = m_Impl->m_CurrentFrame->ByteSize();
		/// check that our frame and the native have the same byte size
		DebugAssertMsg(bsize == m_Impl->m_CurrentFrame->ByteSize(), "Decoded native frame type has different byte size.");
		/// copy native frame data to our buffer
		BufferCPU<uchar4> currentFrame = CreateBufferCPU<uchar4>(m_Impl->m_CurrentFrame->Count());
		std::memcpy(currentFrame->Data(), m_Impl->m_CurrentFrameNative.data, m_Impl->m_FrameByteSize);
		/// notify subscriber's about the current frame
		m_Impl->m_CurrentFramePosition = (size_t)m_Impl->m_Decoder->get(cv::CAP_PROP_POS_FRAMES);
		m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(std::make_pair(m_Impl->m_CurrentFramePosition, currentFrame));
		m_Impl->m_NativeFrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_CurrentFrameNative);
	}
}

BufferCPU<uchar4> DecodingNodeObj::GetFrame(int id)
{
	this->SetCurrentFramePos(id);
	if (m_Impl->m_Decoder->isOpened() && m_Impl->m_CurrentFramePosition < m_Impl->m_FrameCount)
	{

		m_Impl->m_Decoder->read(m_Impl->m_CurrentFrameNative);
		cv::cvtColor(m_Impl->m_CurrentFrameNative, m_Impl->m_CurrentFrameNative, cv::COLOR_BGR2RGBA);
		/// get the byte size of the native frame
		size_t bsize = m_Impl->m_CurrentFrameNative.total() * m_Impl->m_CurrentFrameNative.elemSize();
		size_t fbsize = m_Impl->m_CurrentFrame->ByteSize();
		/// check that our frame and the native have the same byte size
		DebugAssertMsg(bsize == m_Impl->m_CurrentFrame->ByteSize(), "Decoded native frame type has different byte size.");
		/// copy native frame data to our buffer
		if (m_Impl->m_DebugFramesEnabled)
		{
			#if _MSC_VER >= 1924
				using namespace std;
			#else		
				using namespace std::experimental;
			#endif
			std::stringstream ss;
			std::string vfname = filesystem::path(m_Impl->m_LoadedFile).filename().replace_extension("").generic_string();
			ss << m_Impl->m_DebugOutDir << "\\" << vfname << "_distorted_" << std::setw(4) << std::setfill('0') << std::to_string(m_Impl->m_CurrentFramePosition) << ".png";
			//std::string fname = m_Impl->m_DebugOutDir + "\\" + vfname + "_distorted_" + std::to_string(m_Impl->m_CurrentFramePosition) + ".png";
			cv::Mat out;
			cv::cvtColor(m_Impl->m_CurrentFrameNative, out, cv::COLOR_RGB2BGR);
			cv::imwrite(ss.str(), out);
		}
		if (m_Impl->m_Undistort)
		{
			try
			{
				cv::Mat newCamMat = 
					cv::getOptimalNewCameraMatrix(m_Impl->m_CameraMatrix, 
						m_Impl->m_DistCoeffs, 
						cv::Size(m_Impl->m_CurrentFrameNative.cols, m_Impl->m_CurrentFrameNative.rows), 
						0, 
						cv::Size(m_Impl->m_CurrentFrameNative.cols, m_Impl->m_CurrentFrameNative.rows));
				cv::Mat undistorted;
				cv::undistort(m_Impl->m_CurrentFrameNative, undistorted, m_Impl->m_CameraMatrix, m_Impl->m_DistCoeffs, newCamMat);
				m_Impl->m_CurrentFrameNative = undistorted;
			}
			catch (std::exception& ex)
			{
				LOG_ERROR << ex.what();
			}
			if (m_Impl->m_DebugFramesEnabled)
			{
				#if _MSC_VER >= 1924
					using namespace std;
				#else		
					using namespace std::experimental;
				#endif
				std::stringstream ss;
				std::string vfname = filesystem::path(m_Impl->m_LoadedFile).filename().replace_extension("").generic_string();
				ss << m_Impl->m_DebugOutDir << "\\" << vfname << "_undistorted_" << std::setw(4) << std::setfill('0') << std::to_string(m_Impl->m_CurrentFramePosition) << ".png";
				//std::string fname = m_Impl->m_DebugOutDir + "\\" + vfname + "_undistorted_" + std::to_string(m_Impl->m_CurrentFramePosition) + ".png";
				cv::Mat out;
				cv::cvtColor(m_Impl->m_CurrentFrameNative, out, cv::COLOR_RGB2BGR);
				cv::imwrite(ss.str(), out);
			}
		}
		if (m_Impl->m_Rotate)
		{
			double angle = -90.0;
			cv::Point2f center((m_Impl->m_CurrentFrameNative.cols - 1.0f) / 2.0f, (m_Impl->m_CurrentFrameNative.rows - 1.0f) / 2.0f);
			cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0f);
			cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), m_Impl->m_CurrentFrameNative.size(), angle).boundingRect2f();
			///
			rot.at<double>(0, 2) += bbox.width / 2.0f - m_Impl->m_CurrentFrameNative.cols / 2.0;
			rot.at<double>(1, 2) += bbox.height / 2.0f - m_Impl->m_CurrentFrameNative.rows / 2.0;

			cv::warpAffine(m_Impl->m_CurrentFrameNative, m_Impl->m_CurrentFrameNative, rot, bbox.size());
		}
		std::memcpy(m_Impl->m_CurrentFrame->Data(), m_Impl->m_CurrentFrameNative.data, m_Impl->m_FrameByteSize);
		
		/// notify subscriber's about the current frame
		return m_Impl->m_CurrentFrame;
	}
}
///	\brief generate a specific number of frames
///	generates the specified 6number of frames
///	\param	frameCount	the number of frames to generate
///	\note generates the specified number of frames from the current frame position
void DecodingNodeObj::GenerateFrames(size_t frameCount)
{
	if (m_Impl->m_Decoder->isOpened() && m_Impl->m_CurrentFramePosition < m_Impl->m_FrameCount)
	{
		LOG_DEBUG << "Generating frames on thread: " << std::this_thread::get_id();
		unsigned int counter = 0;
		bool gen = m_Impl->m_GenerateFrames.load(std::memory_order_seq_cst);

		m_Impl->m_CurrentFramePosition = (size_t)m_Impl->m_Decoder->get(cv::CAP_PROP_POS_FRAMES);
		m_Impl->m_GenerateFrames.store(true, std::memory_order::memory_order_seq_cst);
		while (counter < frameCount) 
		{
			if (m_Impl->m_ChromaKeyBGEnabled)
			{
				m_Impl->m_CurrentFrameNative = cv::Mat(cv::Size(m_Impl->m_FrameWidth, m_Impl->m_FrameHeight), CV_8UC4, cv::Scalar(m_Impl->m_ChromaKey.x, m_Impl->m_ChromaKey.y, m_Impl->m_ChromaKey.z, m_Impl->m_ChromaKey.w));
				size_t bsize = m_Impl->m_CurrentFrameNative.total() * m_Impl->m_CurrentFrameNative.elemSize();
				size_t fbsize = m_Impl->m_CurrentFrame->ByteSize();
				BufferCPU<uchar4> currentFrame = CreateBufferCPU<uchar4>(m_Impl->m_CurrentFrame->Count());
				std::memcpy(currentFrame->Data(), m_Impl->m_CurrentFrameNative.data, m_Impl->m_FrameByteSize);
				m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(std::make_pair(m_Impl->m_CurrentFramePosition, currentFrame));
				m_Impl->m_NativeFrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_CurrentFrameNative);
				m_Impl->m_CurrentFramePosition = (size_t)m_Impl->m_Decoder->get(cv::CAP_PROP_POS_FRAMES);
			}
			if (m_Impl->m_Decoder->read(m_Impl->m_CurrentFrameNative))
			{
				cv::cvtColor(m_Impl->m_CurrentFrameNative, m_Impl->m_CurrentFrameNative, cv::COLOR_BGR2RGBA);
					/// get the byte size of the native frame
				size_t bsize = m_Impl->m_CurrentFrameNative.total() * m_Impl->m_CurrentFrameNative.elemSize();
				size_t fbsize = m_Impl->m_CurrentFrame->ByteSize();
				/// check that our frame and the native have the same byte size
				DebugAssertMsg(bsize == m_Impl->m_CurrentFrame->ByteSize(), "Decoded native frame type has different byte size.");
				BufferCPU<uchar4> currentFrame = CreateBufferCPU<uchar4>(m_Impl->m_CurrentFrame->Count());
				/// copy native frame data to our buffer
				std::memcpy(currentFrame->Data(), m_Impl->m_CurrentFrameNative.data, m_Impl->m_FrameByteSize);
				///
				//m_Impl->m_CurrentFramePosition = (size_t)m_Impl->m_Decoder->get(cv::CAP_PROP_POS_FRAMES);
				m_Impl->m_FrameFlowOutSubj.get_subscriber().on_next(std::make_pair(m_Impl->m_CurrentFramePosition, currentFrame));
				m_Impl->m_NativeFrameFlowOutSubj.get_subscriber().on_next(m_Impl->m_CurrentFrameNative);
				m_Impl->m_CurrentFramePosition = (size_t)m_Impl->m_Decoder->get(cv::CAP_PROP_POS_FRAMES);
				///	increment counter
				counter++;
			}
			else
			{
				break;
			}
		}
		m_Impl->m_GenerateFrames.store(false, std::memory_order::memory_order_seq_cst);
		LOG_DEBUG << "Generation finished on thread: " << std::this_thread::get_id() << "counter: " << counter;
		m_Impl->m_GenerateFramesTaskCompetedSubj.get_subscriber().on_next(nullptr);
	}
}
///	\brief set scaled size
///	\param	width	the new width of the decoded frame
///	\param	height	the new height of the decoded frame
void fu::trans::DecodingNodeObj::SetScaledSize(size_t width, size_t height)
{
	m_Impl->m_ScaledFrameWidth = width;
	m_Impl->m_ScaledFrameHeight = height;
}

void fu::trans::DecodingNodeObj::SetGeneratingFrames(bool val)
{
	m_Impl->m_GenerateFrames.store(val, std::memory_order_seq_cst);
}

bool DecodingNodeObj::IsGenerating() const
{
	return m_Impl->m_GenerateFrames.load(std::memory_order_seq_cst);
}

void DecodingNodeObj::RotateFrames90CW(bool rot)
{
	m_Impl->m_Rotate = rot;
}
void DecodingNodeObj::SetCameraMatrix(const BufferCPU<float>& mat)
{
	cv::Mat camMat = cv::Mat::zeros(cv::Size(3, 3), CV_32FC1);
	std::memcpy(camMat.data, mat->Data(), 9 * sizeof(float));
	m_Impl->m_CameraMatrix = camMat;

}
void DecodingNodeObj::SetDisrtionCoefficients(const DistCoeffs & coeffs)
{
	cv::Mat dist = cv::Mat::zeros(cv::Size(8, 1), CV_32FC1);
	dist.at<float>(0, 0) = coeffs.k1;
	dist.at<float>(0, 1) = coeffs.k2;
	dist.at<float>(0, 2) = coeffs.p1;
	dist.at<float>(0, 3) = coeffs.p2;
	dist.at<float>(0, 4) = coeffs.k3;
	dist.at<float>(0, 5) = coeffs.k4;
	dist.at<float>(0, 6) = coeffs.k5;
	dist.at<float>(0, 7) = coeffs.k6;
	m_Impl->m_DistCoeffs = dist;
}
void DecodingNodeObj::SetUndistortEnabled(bool enabled)
{
	m_Impl->m_Undistort = enabled;
}
void DecodingNodeObj::SetSaveDebugFramesEnabled(bool enabled)
{
	m_Impl->m_DebugFramesEnabled = enabled;
}
void DecodingNodeObj::SetDebugFramesOutDir(const std::string & outdir)
{
	m_Impl->m_DebugOutDir = outdir;
}
void DecodingNodeObj::SetChromaKeyBGEnabled(bool enabled)
{
	m_Impl->m_ChromaKeyBGEnabled = enabled;
}
void DecodingNodeObj::SetChromaKeyBGVal(const uchar4& chroma)
{
	m_Impl->m_ChromaKey = chroma;
}
///	\brief frame output
///	decoding nodes have only output frame streams
rxcpp::observable<DecodingNodeObj::frame_out_t> DecodingNodeObj::FrameFlowOut()
{
	return m_Impl->m_FrameFlowOutSubj.get_observable().as_dynamic();
}
///	\brief native frame output
///	native frame output for hooking in other nodes
rxcpp::observable<DecodingNodeObj::native_frame_t> DecodingNodeObj::NativeFrameFlowOut()
{
	return m_Impl->m_NativeFrameFlowOutSubj.get_observable().as_dynamic();
}
///	\brief generate the specified amount of frames
rxcpp::observer<size_t> fu::trans::DecodingNodeObj::PrefetchFrames()
{
	return m_Impl->m_GenerateFramesTask.get_subscriber().get_observer().as_dynamic();
}
///	\brief generate frames task completed notification
rxcpp::observable<void*> fu::trans::DecodingNodeObj::PrefetchFramesCompleted()
{
	return m_Impl->m_GenerateFramesTaskCompetedSubj.get_observable().as_dynamic();
}
}	///	!namespace trans
}	///	!namespace fu