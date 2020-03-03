//#include <Transcoder.h>
#include <DebugMsg.h>
#include <plog/Log.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/videostab.hpp>

#include <rxcpp/rx.hpp>

//namespace fu {
//namespace trans {
/////	\typedef frame_t
/////	\brief the internal type of a frame
//	using frame_t = std::shared_ptr<cv::Mat>;
/////	\struct DecodingContextObj
/////	\brief a decoding context
//struct DecodingContextObj
//{
//	/// an opencv video decoder
//	cv::VideoCapture m_Decoder;
//	/// frame length
//	int				m_FrameCount{ 0 };
//	/// input media stream's frame rate
//	double			m_FrameRate{ 0.0 };
//	/// Width of the input media file's frames
//	int				m_FrameWidth{ 0 };
//	/// Height of the input media file's frames
//	int				m_FrameHeight{ 0 };
//	///	current frame buffer
//	frame_t			m_CurrentFrame;
//	/// current decoded frame id
//	int				m_CurrentFrameId{ 0 };
//	///	frame output
//	rxcpp::subjects::subject<cv::Mat> m_FrameFlowOut;
//	///	subscriptions
//	rxcpp::composite_subscription	m_Subscriptions;
//};
//namespace dec {
/////	\brief create a context
/////	allocates a decodding object pointer
//DecodingContext CreateContext()
//{
//	DecodingContext context = std::make_shared<DecodingContextObj>();
//	return context;
//}
/////	\brief destroy the decoding context
//void DestroyContext(DecodingContext context)
//{
//	context->m_Subscriptions.unsubscribe();
//	context->m_Decoder.release();	
//}
/////	\brief load a meadia file
/////	\param	filepath	the incoming file's path
/////	\return true if the file was loaded successfully, false otherwise
//bool LoadFile(DecodingContext context, const std::string& filepath)
//{
//	if (context->m_Decoder.isOpened())
//		context->m_Decoder.release();
//
//	if (!context->m_Decoder.open(filepath))
//	{
//		LOG_ERROR << "Failed to load media file: " << filepath;
//		return false;
//	}
//	if (context->m_Decoder.isOpened())
//	{
//		context->m_CurrentFrameId = 0;
//		context->m_FrameCount = context->m_Decoder.get(cv::CAP_PROP_FRAME_COUNT);
//		context->m_FrameRate = context->m_Decoder.get(cv::CAP_PROP_FPS);
//		context->m_FrameWidth = context->m_Decoder.get(cv::CAP_PROP_FRAME_WIDTH);
//		context->m_FrameHeight = context->m_Decoder.get(cv::CAP_PROP_FRAME_HEIGHT);
//	}
//}
//}	///	!namespace dec
/////	\struct ScalingContextObj
///// \brief Context that scales incoming frames and outputs scaled ones
//struct ScalingContextObj
//{
//	/// int input frame width
//	int m_InputFrameWidth;
//	///	int input frame height
//	int m_InputFrameHeight;
//	///	int output frame width
//	int m_OutputFrameWidth;
//	///	int output frame height
//	int m_OutputFrameHeight;
//	///	int scale type
//	/// TODO:
//	///	current scaled frame
//	frame_t m_CurrentFrame;
//	///	frame input
//	rxcpp::subjects::subject<cv::Mat>	m_FrameFlowIn;
//	/// frame output
//	rxcpp::subjects::subject<cv::Mat>	m_FrameFlowOut;
//	/// subscriptions
//	rxcpp::composite_subscription		m_Subscriptions;
//};
//namespace scale {
/////	\brief create a scaling context
/////	\return a new scaling context
//ScalingContext CreateContext()
//{
//	ScalingContext context = std::make_shared<ScalingContextObj>();
//	return context;
//}
/////	\brief destroy the scaling context
//void DestroyContext(ScalingContext context)
//{
//	context->m_Subscriptions.unsubscribe();
//}
//}	///	!namespace scale
//}	///	!namespace trans
//}	///	!namespace fu

#if 0
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mftransform.h>
#include <Mferror.h>
#include <atlbase.h>
#include <atlstr.h>
#include <comdef.h>
#endif	///	!_WIN

namespace fu {
namespace trans {
///	\brief log a Windows error
///	Helper function to log windows errors
///	\param	hr		the hresult received from the operation
void LogError(HRESULT hr)
{
	DWORD last = ::GetLastError();
	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		last,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&messageBuffer,
		0, NULL);
	std::string msg(messageBuffer, size);
	LOG_ERROR << msg;
	::SetLastError(0);
}
}	///	!namespace trans
}	///	!namespace fu
/// wrapper over mf call checking macro
#define CHECK_MFCALL(call, hr)		\
	if (hr = call != S_OK)			\
	{								\
		fu::trans::LogError(hr);	\
		return false;				\
	}
/// wraper that throws a runtime error if the given pointer is null
#define BREAK_ON_NULLPTR(ptr)													\
	if (ptr == NULL)	throw std::runtime_error("The given pointer is NULL.");

namespace fu {
namespace trans {
///	\struct FrameRate
///	\brief a simple rational that represents a frame rate
struct FrameRate
{
	UINT32 Numerator;
	UINT32 Denominator;
};	///	!struct FrameRate
///	\struct DecodingContextObj
///	\brief Decoding Context implementation
struct DecodingContextObj
{
	/// Source data
	///	the decofing context is thread-safe by using a creitical section
	///	synchronization mechanism
	CComAutoCriticalSection		m_CriticalSection;
	///	media source
	CComPtr<IMFMediaSource>		m_MediaSource;
	///	decoding topology
	CComPtr<IMFTopology>		m_Topology;
	///	audio stream id
	int							m_AudioStreamId;
	///	audio source node in the topology
	///	warning: not an actual source if not explicitly set, otherwise
	///	just a node in the topology
	CComPtr<IMFTopologyNode>	m_LastAudioSourceNode;
	///	video stream id
	int							m_VideoStreamId;
	///	video source node in the topology
	///	warning: not an actual source if not explicitly set, otherwise
	///	just a node in the topology
	CComPtr<IMFTopologyNode>	m_LastVideoSourceNode;
	///	the number of streams the decoding context has extracted
	unsigned int				m_NumStreams{ 0 };
	///	flag to signify that the decoding context has an audio stream
	bool m_HasAudioStream{ false };
	///	flag to signify that the decoding context has a video stream
	bool m_HasVideoStream{ false };
	/// the audio decoder MFTransform
	CComPtr<IMFTransform>		m_AudioDecoder;
	GUID						m_AudioDecoderSubType;
	CLSID						m_AudioDecoderCLSID;
	CComPtr<IMFTopologyNode>	m_AudioDecoderNode;
	unsigned int				m_AudioNumChannels;
	/// the video decoder MFTransform
	CComPtr<IMFTransform>		m_VideoDecoder;
	GUID						m_VideoDecoderSubType;
	CLSID						m_VideoDecoderCLSID;
	CComPtr<IMFTopologyNode>	m_VideoDecoderNode;
	FrameRate					m_VideoSourceFrameRate;
	/// The context's worker thread
	DWORD						m_SyncMftWorkerQueue;
	CComPtr<IMFMediaEventQueue>	m_EventQueue;
	HANDLE						m_AudioSourceSampleReadyEvent;
	HANDLE						m_VideoSourceSampleReadyEvent;
	CComPtr<IMFSample>			m_CurrentReadySample;
	/// Playback and Seeking related
	size_t							m_CurrentSample;
	CComPtr<IMFPresentationClock>	m_Clock;
	///	flag to indicate playback started
	bool						m_IsInitialized{ false };
	
};
namespace dec {
///	\brief create a context
///	allocates a decodding object pointer
DecodingContext CreateContext()
{
	HRESULT hr = MFStartup(MF_VERSION);
	return std::make_shared<DecodingContextObj>();
}
///	\brief destroy the decoding context
void DestroyContext(DecodingContext context)
{
	// TODO:
}
///	\brief get the decoder category of the specified decoder major type
///	\param	majorType	the source's major type (audio, video, cc)
///	\param[out]	Category	a pointer to a GUID to fill with the appropriate decoder GUID
///	\return S_OK if successful codec retrieval, S_FALSE otherwise
HRESULT GetDecoderMajorCategory(const GUID& majorType, GUID* outCategory)
{
	if (majorType == MFMediaType_Audio)
	{
		*outCategory = MFT_CATEGORY_AUDIO_DECODER;
	}
	else if (majorType == MFMediaType_Video)
	{
		*outCategory = MFT_CATEGORY_VIDEO_DECODER;
	}
	else
	{
		return S_FALSE;
	}
	return S_OK;
}
///	\brief load a meadia file
///	\param	filepath	the incoming file's path
///	\return true if the file was loaded successfully, false otherwise
bool LoadFile(DecodingContext context, const std::wstring& filepath)
{
	HRESULT hr;
	LPCWSTR widefilepath = (LPWSTR)filepath.c_str();
	CComPtr<IMFSourceResolver>	spResolver;
	CComPtr<IUnknown>			spSrcUnk;
	CComPtr<IMFPresentationDescriptor>	spPrDesc;
	/// create a topology for the context
	CHECK_MFCALL(hr = MFCreateTopology(&context->m_Topology), hr);

	MF_OBJECT_TYPE ObjectType;
	CHECK_MFCALL(hr = MFCreateSourceResolver(&spResolver), hr);
	CHECK_MFCALL(hr = spResolver->CreateObjectFromURL(widefilepath, MF_RESOLUTION_MEDIASOURCE, nullptr, &ObjectType, &spSrcUnk), hr);
	CHECK_MFCALL(hr = spSrcUnk->QueryInterface(IID_PPV_ARGS(&context->m_MediaSource)), hr);
	CHECK_MFCALL(hr = context->m_MediaSource->CreatePresentationDescriptor(&spPrDesc), hr);
	/// count streams in media file
	DWORD sdCount;
	CHECK_MFCALL(hr = spPrDesc->GetStreamDescriptorCount(&sdCount), hr);
	/// find the streams in the media file
	for (int i = 0; i < sdCount; i++)
	{
		CHECK_MFCALL(hr = spPrDesc->SelectStream(i), hr);

		CComPtr<IMFStreamDescriptor> spStreamDesc;
		BOOL selected;
		CHECK_MFCALL(hr = spPrDesc->GetStreamDescriptorByIndex(i, &selected, &spStreamDesc), hr);
		/// Each source node needs to be set up eith MF_TOPNODE_SOURCE
		///	MF_TOPNODE_PRESENTATION_DESCRIPTOR, and MF_TOPPNODE_STREAM_DESCRIPTOR
		/// for the MF pipeline to unambiguously identify what stream is 
		/// associated with the topology
		CComPtr<IMFTopologyNode> spSourceStreamNode;
		CHECK_MFCALL(hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &spSourceStreamNode), hr);
		CHECK_MFCALL(hr = spSourceStreamNode->SetUnknown(MF_TOPONODE_SOURCE, context->m_MediaSource), hr);
		CHECK_MFCALL(hr = spSourceStreamNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, spPrDesc), hr);
		CHECK_MFCALL(hr = spSourceStreamNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, spStreamDesc), hr);
		CHECK_MFCALL(hr = context->m_Topology->AddNode(spSourceStreamNode), hr);
		
		CComPtr<IMFMediaTypeHandler> spMediaTypeHandler;
		CHECK_MFCALL(hr = spStreamDesc->GetMediaTypeHandler(&spMediaTypeHandler), hr);

		CComPtr<IMFMediaType> spMediaType;
		CHECK_MFCALL(hr = spMediaTypeHandler->GetCurrentMediaType(&spMediaType), hr);
		
		GUID guidMajorType;
		CHECK_MFCALL(hr = spMediaType->GetMajorType(&guidMajorType), hr);
		/// Check for media sub type
		GUID guidSubType;
		CHECK_MFCALL(hr = spMediaType->GetGUID(MF_MT_SUBTYPE, &guidSubType), hr);
		/// check if any media stream is compressed
		BOOL isCompressed = FALSE;
		GUID guidDecoderCategory;
		CLSID* decoderCLSIDArray;
		UINT32 decoderCLSIDArraySize;
		/// get the first CLSID in the decoder CLSID array
		CLSID decoderCLSID;
		CHECK_MFCALL(hr = spMediaType->IsCompressedFormat(&isCompressed), hr);
		if (isCompressed)
		{
			/// Select decoder category from major type
			CHECK_MFCALL(hr = GetDecoderMajorCategory(guidMajorType, &guidDecoderCategory), hr);
			/// Look for a decoder
			MFT_REGISTER_TYPE_INFO tinfo;
			tinfo.guidMajorType = guidMajorType;
			tinfo.guidSubtype = guidSubType;
			CHECK_MFCALL(hr = MFTEnum(guidDecoderCategory, 0, &tinfo, NULL, NULL, &decoderCLSIDArray, &decoderCLSIDArraySize), hr);
			if (decoderCLSIDArraySize == 0)
			{
				LOG_ERROR << "Failed to find apropriate decoder for file: " << filepath;
				return false;
			}
			decoderCLSID = decoderCLSIDArray[0];
		}
		else
		{
			decoderCLSID = GUID_NULL;
		}
		/// create a decoder topology node
		CComPtr<IMFTopologyNode> decoderNode;
		CHECK_MFCALL(hr = MFCreateTopologyNode(MF_TOPOLOGY_TRANSFORM_NODE, &decoderNode), hr);
		CHECK_MFCALL(hr = decoderNode->SetGUID(MF_TOPONODE_TRANSFORM_OBJECTID, decoderCLSID), hr);
		/// add the node to the context topology
		CHECK_MFCALL(hr = context->m_Topology->AddNode(decoderNode), hr);
		/// fill the appropriate stream values in the context
		if (guidMajorType == MFMediaType_Video)
		{
			context->m_LastVideoSourceNode = spSourceStreamNode;
			context->m_HasVideoStream = true;
			context->m_VideoDecoderSubType = guidSubType;
			context->m_VideoDecoderCLSID = decoderCLSID;
			context->m_VideoDecoderNode = decoderNode;
			/// connect the video stream source node to the decoder's sink node
			CHECK_MFCALL(hr = context->m_LastVideoSourceNode->ConnectOutput(0, context->m_VideoDecoderNode, 0), hr);
			CHECK_MFCALL(hr = context->m_VideoDecoderNode->SetUINT32(MF_TOPONODE_CONNECT_METHOD, MF_CONNECT_ALLOW_CONVERTER), hr);
			/// create video source ready event
			context->m_VideoSourceSampleReadyEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
			/// get the video source's frame rate
			//CHECK_MFCALL(hr = MFGetAttributeRatio(spMediaType, MF_MT_FRAME_RATE, &context->m_VideoSourceFrameRate.Numerator, &context->m_VideoSourceFrameRate.Denominator), hr);
		}
		///	Find Audio decoder
		else if (guidMajorType == MFMediaType_Audio)
		{
			context->m_LastAudioSourceNode = spSourceStreamNode;
			context->m_HasAudioStream = true;
			context->m_AudioDecoderSubType = guidSubType;
			context->m_AudioDecoderCLSID = decoderCLSID;
			context->m_AudioDecoderNode = decoderNode;
			/// connect the video stream source node to the decoder's sink node
			CHECK_MFCALL(hr = context->m_LastAudioSourceNode->ConnectOutput(0, context->m_AudioDecoderNode, 0), hr);
			CHECK_MFCALL(hr = context->m_AudioDecoderNode->SetUINT32(MF_TOPONODE_CONNECT_METHOD, MF_CONNECT_ALLOW_CONVERTER), hr);
			context->m_AudioSourceSampleReadyEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
			/// get the video source's frame rate
			//CHECK_MFCALL(hr = MFGetAttributeUINT32(spMediaType, MF_MT_AUDIO_NUM_CHANNELS, context->m_AudioNumChannels), hr);
		}
		context->m_NumStreams++;
	}
	/// allocate a special worker thread for the blocking synchronous MFT operations
	CHECK_MFCALL(hr = MFAllocateWorkQueue(&context->m_SyncMftWorkerQueue), hr);
	/// create the context's event queue
	CHECK_MFCALL(hr = MFCreateEventQueue(&context->m_EventQueue), hr);
	context->m_IsInitialized = true;
	return context->m_IsInitialized;
}
///	\brief check if the decoding context has a video source
///	\param	context	the context to check
///	\return true if the incoming context has a video steam
bool HasVideoSource(DecodingContext context)
{
	return context->m_HasVideoStream;
}
///	\brief check if the decoding context has an audio source
///	\param	context	the context to check
///	\return true if the incoming context has a video steam
bool HasAudioSource(DecodingContext context)
{
	return context->m_HasAudioStream;
}
///	\brief pull data from a Video Source
///	\param	context	the context to pull data from
///	\param[out] sample	the sample to put data into
///	\return true if data were successfully pulled, false otherwise
bool PullDataFromVideoSource(DecodingContext context, IMFSample** sample)
{
	HRESULT hr = S_OK;
	if (context->m_HasVideoStream)
	{
		/// signal the source stream that we need a new sample
		CHECK_MFCALL(hr = context->m_MediaSource->)
	}
	
}
///	\brief pull data from the decoding context's decoders
///	\param	context	the context to pull data from the decoders
///	\param	sample	the sample to put the pulled data into
/// \return	true if the data where pulled successfully, false otherwise
bool PullDataFromDecoder(DecodingContext context, IMFSample** sample)
{
	HRESULT hr  = S_OK;
	MFT_OUTPUT_DATA_BUFFER outputDataBuffer;
	DWORD processOutputStatus = 0;
	CComPtr<IMFSample> mftInputSample;
	if (context->m_HasVideoStream)
	{
		BREAK_ON_NULLPTR(context->m_VideoDecoder);
		BREAK_ON_NULLPTR(sample);
		/// initialize the video decoder buffer
		// CHECK_MFCALL(hr = InitOutputDataBa)

		/// try to get output from the video decoder MFT.
		///	if the decoder returns that it needs input, get the data from the
		///	upstream component and send it to the decoder's input, and try again.
		while (true)
		{
			/// try to get output sample from the decoder
			CHECK_MFCALL(hr = context->m_VideoDecoder->ProcessInput(0, mftInputSample, 0), hr);
		}
	}
}
///	\param	context
/// \brief handles a synchronous MFT request
///	\note: will block while it waits for data
///	\return true if the MFT request handled successflly, false otherwise
bool HandleSynchronousMftRequest(DecodingContext context)
{
	HRESULT hr = S_OK;
	CComPtr<IMFSample> Sample;
	CComCritSecLock<CComCriticalSection> lock(context->m_CriticalSection);
	/// get data from the video decoder
	if (context->m_HasVideoStream)
	{
		// CHECK_MFCALL()
	}
	/// get data from the audio stream
	if (context->m_HasAudioStream)
	{

	}
	return true;
}
///	\brief set the decoder to a specified position in the stream
///	\param	inContext	the decoding context to process
///	\param	inSampleId	the position of the sample to set the decoding context
bool GoToSample(DecodingContext context, const unsigned int inSampleId)
{
	for (unsigned int i = 0; i < context->m_NumStreams; i++)
	{

	}
	return true;
}
}	///	!namespace dec
///	\struct TranscodingContext
///	\brief transcoding context state (opaque to the user)
struct TranscodingContextObj
{
	std::string			LoadedFile;
	HRESULT				MFStartup;
	HRESULT				MFShutdown;
	IMFAttributes*		MFAttributesInterface;
	IMFMediaSession*	MFMediaSession;
};	///	!class TranscodingContext
}	///	!namespace trans
}	///	!namespace fu

#endif