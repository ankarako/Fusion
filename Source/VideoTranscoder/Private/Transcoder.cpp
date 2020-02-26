#include <Transcoder.h>

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
	CComPtr<IMFMediaSource>		m_MediaSource;
	CComPtr<IMFTopology>		m_Topology;
	CComPtr<IMFTopologyNode>	m_LastAudioNode;
	CComPtr<IMFTopologyNode>	m_LastVideoNode;
	CComPtr<IMFTopologyNode>	m_AudioStreamSinkNode;
	CComPtr<IMFTopologyNode>	m_VideoStreamSinkNode;
	bool m_HasAudioStream;
	bool m_HasVideoStream;
};
namespace dec {
///	\brief create a context
///	allocates a decodding object pointer
DecodingContext CreateContext()
{
	HRESULT hr = MFStartup(MF_VERSION);
	return std::make_shared<DecodingContextObj>();
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

	for (int i = 0; i < sdCount; i++)
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