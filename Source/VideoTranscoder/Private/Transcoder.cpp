#include <Transcoder.h>
#include <DebugMsg.h>

#if defined(_WIN32) || defined(_WIN64)
	#include <Windows.h>
	#include <mfapi.h>
	#include <mfidl.h>
	#include <atlbase.h>
	#include <atlstr.h>
	#include <comdef.h>
#endif	///	!_WIN

#include <plog/Log.h>

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
	///	audio source node in the topology
	///	warning: not an actual source if not explicitly set, otherwise
	///	just a node in the topology
	CComPtr<IMFTopologyNode>	m_LastAudioNode;
	///	video source node in the topology
	///	warning: not an actual source if not explicitly set, otherwise
	///	just a node in the topology
	CComPtr<IMFTopologyNode>	m_LastVideoNode;
	///	audio stream sink node
	///	warning: not an actual sink node if not explicitly set
	///	otherwise just a simple node in the topology
	CComPtr<IMFTopologyNode>	m_AudioStreamSinkNode;
	///	video stream sink node
	///	warning: not an actual sink node if not explicitly set
	///	otherwise just a simple node in the topology
	CComPtr<IMFTopologyNode>	m_VideoStreamSinkNode;
	///	flag to signify that the decoding context has an audio stream
	bool m_HasAudioStream;
	///	flag to signify that the decoding context has a video stream
	bool m_HasVideoStream;
	/// the audio decoder MFTransform
	CComPtr<IMFTransform>		m_AudioDecoder;
	/// the video decoder MFTransform
	CComPtr<IMFTransform>		m_VideoDecoder;
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
///	\brief find the appropriate decoder for a stream
///	\note: if the stream is not compressed, outCLSID received the value GUID_NULL
///	\param	StreamDesc	the stream's descriptor
///	\param[out] pitCLSID	a pointer to a CLSID object to fill with the appropriate decoder type for the given stream
///	\return S_OK if successful codec retrieval, S_FALSE otherwise
HRESULT FindDecoderForStream(IMFStreamDescriptor* StreamDesc, CLSID* outCLSID)
{

}
///	\brief load a meadia file
///	\param	filepath	the incoming file's path
///	\return true if the file was loaded successfully, false otherwise
bool LoadSource(DecodingContext context, const std::wstring& filepath)
{
	HRESULT hr;
	LPCWSTR widefilepath = (LPWSTR)filepath.c_str();
	CComPtr<IMFSourceResolver>	spResolver;
	CComPtr<IUnknown>			spSrcUnk;
	CComPtr<IMFPresentationDescriptor>	spPrDesc;
	/// create a topology for the context
	if (hr = MFCreateTopology(&context->m_Topology) != S_OK)
	{
		LogError(hr);
		return false;
	}

	MF_OBJECT_TYPE ObjectType;
	if (hr = MFCreateSourceResolver(&spResolver) != S_OK)
	{
		LogError(hr);
		return false;
	}
	if (hr = spResolver->CreateObjectFromURL(widefilepath, MF_RESOLUTION_MEDIASOURCE, nullptr, &ObjectType, &spSrcUnk) != S_OK)
	{
		LogError(hr);
		return false;
	}
	if (hr = spSrcUnk->QueryInterface(IID_PPV_ARGS(&context->m_MediaSource)) != S_OK)
	{
		LogError(hr);
		return false;
	}
	if (hr = context->m_MediaSource->CreatePresentationDescriptor(&spPrDesc) != S_OK)
	{
		LogError(hr);
		return false;
	}
	/// count streams in media file
	DWORD sdCount;
	if (hr = spPrDesc->GetStreamDescriptorCount(&sdCount) != S_OK)
	{
		LogError(hr);
		return false;
	}
	/// find the streams in the media file
	for (int i = 0; i < sdCount; i++)
	{
		if (hr = spPrDesc->SelectStream(i) != S_OK)
		{
			LogError(hr);
		}
		CComPtr<IMFStreamDescriptor> spStreamDesc;
		BOOL selected;
		if (hr = spPrDesc->GetStreamDescriptorByIndex(i, &selected, &spStreamDesc) != S_OK)
		{
			LogError(hr);
			return false;
		}
		/// Each source node needs to be set up eith MF_TOPNODE_SOURCE
		///	MF_TOPNODE_PRESENTATION_DESCRIPTOR, and MF_TOPPNODE_STREAM_DESCRIPTOR
		/// for the MF pipeline to unambiguously identify what stream is 
		/// associated with the topology
		CComPtr<IMFTopologyNode> spSourceStreamNode;
		if (hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &spSourceStreamNode) != S_OK)
		{
			LogError(hr);
			return false;
		}
		if (hr = spSourceStreamNode->SetUnknown(MF_TOPONODE_SOURCE, context->m_MediaSource) != S_OK)
		{
			LogError(hr);
			return false;
		}
		if (hr = spSourceStreamNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, spPrDesc) != S_OK)
		{
			LogError(hr);
			return false;
		}
		if (hr = spSourceStreamNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, spStreamDesc) != S_OK)
		{
			LogError(hr);
			return false;
		}
		if (hr = context->m_Topology->AddNode(spSourceStreamNode) != S_OK)
		{
			LogError(hr);
			return false;
		}

		CComPtr<IMFMediaTypeHandler> spMediaTypeHandler;
		if (hr = spStreamDesc->GetMediaTypeHandler(&spMediaTypeHandler) != S_OK)
		{
			LogError(hr);
			return false;
		}

		CComPtr<IMFMediaType> spMediaType;
		if (hr = spMediaTypeHandler->GetCurrentMediaType(&spMediaType) != S_OK)
		{
			LogError(hr);
			return false;
		}
		
		GUID gidMajorType;
		if (hr = spMediaType->GetMajorType(&gidMajorType) != S_OK)
		{
			LogError(hr);
			return false;
		}

		if (gidMajorType == MFMediaType_Video)
		{
			context->m_LastVideoNode = spSourceStreamNode;
			context->m_HasVideoStream = true;
		}
		else if (gidMajorType == MFMediaType_Audio)
		{
			context->m_LastAudioNode = spSourceStreamNode;
			context->m_HasAudioStream = true;
		}
	}
	/// Get the source's metadata and codecs for creating the decoding context's
	///	decoder MF Transforms
	
	return true;
}
IUnknown* Transform2IUnknown()
{

}
///	\brief add a video transform to the decoding context
///	\param	Transform	the transform to add
///	\return true if the transform was added successfully, false otherwise
bool AddTransform()
{

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
///	\brief create a TranscodingContext
///	\param	version	the version of the context to create
///	\return true if the context was created successfully, false otherwise
//DecodingContext CreateDecodingContext()
//{
//	//TranscodingContext context = 
//	//	std::make_shared<TranscodingContextObj>();
//	//context->MFStartup = MFStartup(MF_VERSION, MFSTARTUP_LITE);
//	//
//	//if (context->MFStartup != S_OK)
//	//	throw std::exception("Failed to create transcoding context: Windows Media Foundation is not implemented on this system.");	
//	//
//	//return context;
//}
/////	\brief shutdown a transcoding context
/////	Releases the Windows Media Foundation context
//void DestroyDecodingContext(DecodingContext context)
//{
//	//context->MFShutdown = MFShutdown();
//}
}	///	!namespace trans
}	///	!namespace fu