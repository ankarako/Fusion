#include <Models/OmniConnectModel.h>
#include <Core/SettingsRepo.h>
#include <Settings/ProjectSettings.h>
#include <Settings/OmniconnectSettings.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/filestream.h>
#include <plog/Log.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>
#include <fstream>
#include <vector>
#include <filesystem>

#include <codecvt>
#include <locale>

#include <curl/curl.h>


namespace fu {
namespace fusion {

	/// FIXME: added these here. Really tedious but I don't really care
size_t readfunc(void* ptr, size_t size, size_t nmemb, void* stream)
{
	FILE* f = (FILE*)stream;
	size_t n;

	if (ferror(f))
	{
		return CURL_READFUNC_ABORT;
	}
	n = fread(ptr, size, nmemb, f) * size;
	return n;
}

struct OmniconnectModel::Impl
{
	using omni_set_ptr_t = std::shared_ptr<OmniconnectSettings>;
	srepo_ptr_t				m_Srepo;
	std::string				m_DownloadPath = "";
	prj_set_ptr_t			m_ProjectSettings;
	omni_set_ptr_t			m_Settings;
	omni_video_list_ptr_t	m_VideoList;
	size_t					m_DownloadedVideoSize;
	std::string				m_DownloadedVideoFilepath;
	std::string				m_MentorLayerFilepath;
	rxcpp::subjects::subject<prj_set_ptr_t>			m_ProjectSettingsFlowInSubj;
	rxcpp::subjects::subject<void*>					m_ShowMediaListFlowInSubj;
	rxcpp::subjects::subject<int>					m_VideoListIndexFlowInSubj;
	rxcpp::subjects::subject<std::string>			m_VideoFilepathFlowOutSubj;
	rxcpp::subjects::subject<omni_video_list_ptr_t>	m_VideoListsFlowOutSubj;
	rxcpp::subjects::subject<float>					m_ProgressFlowOutSubj;
	rxcpp::subjects::subject<void*>					m_ExportMentorFileFlowInSubj;
	rxcpp::subjects::subject<void*>					m_UploadMentorVideoFileFlowInSubj;
	rxcpp::subjects::subject<std::string>			m_ExpotMentorLayerFilepathFlowOutSubj;

	curl_off_t SFTPGetRemoteFileSize(const char* i_RemoteFile)
	{
		CURLcode result = CURLE_GOT_NOTHING;
		curl_off_t remoteFileByteSize = -1;
		CURL* curlHandlePtr = curl_easy_init();

		curl_easy_setopt(curlHandlePtr, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curlHandlePtr, CURLOPT_URL, i_RemoteFile);
		curl_easy_setopt(curlHandlePtr, CURLOPT_NOPROGRESS, 1);
		curl_easy_setopt(curlHandlePtr, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curlHandlePtr, CURLOPT_HEADER, 1);
		curl_easy_setopt(curlHandlePtr, CURLOPT_FILETIME, 1);

		result = curl_easy_perform(curlHandlePtr);
		if (CURLE_OK == result)
			result = curl_easy_getinfo(curlHandlePtr, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &remoteFileByteSize);

		if (result)
			return -1;

		curl_easy_cleanup(curlHandlePtr);
		return remoteFileByteSize;
	}

	int SFTPResumeUpload(CURL* curlHandle, const char* remotePath, const char* localPath)
	{
		FILE* fd = nullptr;
		CURLcode result = CURLE_GOT_NOTHING;

		curl_off_t remoteFileByteSize = SFTPGetRemoteFileSize(remotePath);
		if (remoteFileByteSize == -1)
			LOG_ERROR << "Failed to read remote file size.";

		fd = fopen(localPath, "rb");
		if (!fd)
		{
			LOG_ERROR << "Failed to load mentor layer: " << localPath;
			return -1;
		}

		char error[512];
		curl_easy_setopt(curlHandle, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curlHandle, CURLOPT_URL, remotePath);
		curl_easy_setopt(curlHandle, CURLOPT_READFUNCTION, readfunc);
		curl_easy_setopt(curlHandle, CURLOPT_READDATA, fd);
		curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, error);
		curl_easy_setopt(curlHandle, CURLOPT_APPEND, 1L);
		if (remoteFileByteSize != -1)
		{
			_fseeki64(fd, remoteFileByteSize, SEEK_SET);
		}

		result = curl_easy_perform(curlHandle);
		fclose(fd);
		if (result != CURLE_OK)
		{
			LOG_ERROR << "Failed to upload mentor layer: " << error;
			return 0;
		}
		return 1;
	}

	Impl(srepo_ptr_t srepo) 
		: m_Srepo(srepo)
		, m_Settings(std::make_shared<OmniconnectSettings>())

	{ }
};

OmniconnectModel::OmniconnectModel(srepo_ptr_t srepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(srepo))
{ }

void OmniconnectModel::Init()
{
	m_Impl->m_Srepo->RegisterSettings(m_Impl->m_Settings);

	m_Impl->m_ProjectSettingsFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const prj_set_ptr_t& set) { m_Impl->m_ProjectSettings = set; });
	///========================
	///	show media list flow in
	///========================
	m_Impl->m_ShowMediaListFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](auto _) 
	{
		using convert_t = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_t, wchar_t> strConverter;

		bool status = false;
		LOG_INFO << "Invoking Omniconnect server...";
		auto filestream = std::make_shared<concurrency::streams::ostream>();
		
		std::wstring workspaceDir = strConverter.from_bytes(m_Impl->m_ProjectSettings->WorkspacePath);
		std::wstring filename = strConverter.from_bytes(std::string("\\video_list.json"));
		std::wstring outputDir = workspaceDir + filename;
		m_Impl->m_Settings->VideoListResponseFilepath = strConverter.to_bytes(outputDir);
		m_Impl->m_DownloadPath = strConverter.to_bytes(outputDir);

		pplx::task<void> requestMediaListTask =
			concurrency::streams::fstream::open_ostream(outputDir)
			.then([=](concurrency::streams::ostream outFile)
		{
			*filestream = outFile;
			web::http::client::http_client client(U("https://hyper360.api.eng.it/"));
			web::http::uri_builder uriBuilder(U("/GetVideoListsToEnrich"));
			uriBuilder.append_query(U("q"), U("VideoListQuery"));

			return client.request(web::http::methods::GET, uriBuilder.to_string());
		}).then([=, &status](web::http::http_response videoListResponse) {
			LOG_INFO << "Response: " << videoListResponse.status_code();
			if (videoListResponse.status_code() == 200 && videoListResponse.reason_phrase() == L"OK")
				status = true;
			return videoListResponse.body().read_to_end(filestream->streambuf());
		}).then([=](std::size_t)
		{
			filestream->close();
		});
		////////////////////////////////////////////////////////////////////////////////////////////////
		try
		{
			requestMediaListTask.wait();
		}
		catch (std::exception & ex)
		{
			LOG_ERROR << "Failed to invoke Omniconnect server: " << ex.what();
		}

		if (status)
		{
			LOG_INFO << "Reading response message: " << m_Impl->m_DownloadPath;
			/// read the message json
			std::ifstream inFd(m_Impl->m_DownloadPath);
			if (!inFd.is_open())
			{
				LOG_ERROR << "Failed to read file: " << m_Impl->m_DownloadPath;
			}
			rapidjson::IStreamWrapper isw(inFd);
			rapidjson::Document d;
			d.ParseStream(isw);
			if (!d.HasMember("message"))
			{
				LOG_ERROR << "/GetVideoListsToEnrich response is corrupted.";
				return;
			}
			m_Impl->m_VideoList = std::make_shared<std::vector<OmniconnectVideoElement>>();
			auto msgArray = d["message"].GetArray();
			for (const auto& elmnt : msgArray)
			{
				auto values = elmnt.GetArray();
				OmniconnectVideoElement videoElmnt;
				videoElmnt.Uri = values[0].GetString();
				videoElmnt.Name = values[1].GetString();
				videoElmnt.DateAdded = values[2].GetString();
				videoElmnt.UploadUser = values[3].GetString();
				videoElmnt.UploadUri = values[4].GetString();
				m_Impl->m_VideoList->emplace_back(videoElmnt);
			}
			inFd.close();
			m_Impl->m_VideoListsFlowOutSubj.get_subscriber().on_next(m_Impl->m_VideoList);
		}
	});
	///======================================
	/// Video List index in -> Download Video
	///======================================
	m_Impl->m_VideoListIndexFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](int idx) 
	{
		m_Impl->m_Settings->SelectedVideoListIndex = idx;
		using convert_t = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_t, wchar_t> strConverter;
		bool status = false;

		LOG_INFO << "Attempting to download: " << m_Impl->m_VideoList->at(idx).Name;
		
		std::wstring workspaceDir = strConverter.from_bytes(m_Impl->m_ProjectSettings->WorkspacePath);
		std::wstring filename = strConverter.from_bytes("\\" + m_Impl->m_VideoList->at(idx).Name);
		std::wstring uri = strConverter.from_bytes(m_Impl->m_VideoList->at(idx).Uri);
		std::wstring outFilepath = workspaceDir + filename;
		m_Impl->m_DownloadedVideoFilepath = strConverter.to_bytes(outFilepath);
		auto filestream = std::make_shared<concurrency::streams::ostream>();
		///======================
		/// Download Request Task
		///======================
		pplx::task<void> reqDownloadTask =
			concurrency::streams::fstream::open_ostream(outFilepath)
			.then([=](concurrency::streams::ostream outFd)
		{
			*filestream = outFd;
			web::http::client::http_client client(U("https://hyper360.api.eng.it/"));

			web::http::uri_builder builder(uri);
			builder.append_query(U("q"), U("video download query"));
			return client.request(web::http::methods::GET, builder.to_string());
		}).then([=, &status](web::http::http_response response)
		{
			LOG_INFO << "Receved response status code: " << response.status_code();
			if (response.status_code() == 200 && response.reason_phrase() == L"OK")
			{
				status = true;
			}
			m_Impl->m_ProgressFlowOutSubj.get_subscriber().on_next(0.0f);
			pplx::task<size_t> downloadTask = response.body().read_to_end(filestream->streambuf());
			return downloadTask;
		}).then([=](size_t sz) 
		{
			LOG_INFO << "Downloaded " << sz << " bytes.";
			m_Impl->m_ProgressFlowOutSubj.get_subscriber().on_next(1.0f);
			m_Impl->m_DownloadedVideoSize = sz;
			return filestream->close();
		});
		/// Execute task
		try
		{
			reqDownloadTask.wait();
		}
		catch (std::exception & ex)
		{
			LOG_ERROR << "Failed to download video (" << m_Impl->m_VideoList->at(idx).Name << "): " << ex.what();
			m_Impl->m_ProgressFlowOutSubj.get_subscriber().on_next(1.0f);
		}
		/// If successful 
		if (status && m_Impl->m_DownloadedVideoSize != 0)
		{
			m_Impl->m_VideoFilepathFlowOutSubj.get_subscriber().on_next(m_Impl->m_DownloadedVideoFilepath);
		}
	});
	///==========================
	/// Export Mentor Video File 
	///==========================
	m_Impl->m_ExportMentorFileFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](auto _) 
	{
		LOG_INFO << "Request to export mentor layer";
		std::string exportFilename =
			std::filesystem::path(
				m_Impl->m_VideoList->at(m_Impl->m_Settings->SelectedVideoListIndex).Name
			).replace_extension("").generic_string() + "_Mentor";
		m_Impl->m_MentorLayerFilepath = 
			m_Impl->m_ProjectSettings->WorkspacePath 
			+ "\\" + exportFilename;
		LOG_INFO << "Exported video filepath: " << m_Impl->m_MentorLayerFilepath;
		m_Impl->m_ExpotMentorLayerFilepathFlowOutSubj.get_subscriber().on_next(m_Impl->m_MentorLayerFilepath);
	});
	///====================
	/// Upload
	///============
	m_Impl->m_UploadMentorVideoFileFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](auto _)
	{
		LOG_INFO << "Uploading Mentor Layer on Omniconnect server";
		std::string user = m_Impl->m_VideoList->at(m_Impl->m_Settings->SelectedVideoListIndex).UploadUser;
		/// remove ip from user
		
		int slash_idx = user.find("//") + 2;
		int at_idx = user.find('@') - slash_idx;
		std::string username;
		std::string password;
		std::string ip;
		std::string uriToUpload = m_Impl->m_VideoList->at(m_Impl->m_Settings->SelectedVideoListIndex).UploadUri;
		if (at_idx != std::string::npos && slash_idx != std::string::npos)
		{
			username = user.substr(slash_idx, at_idx);
			ip = user.substr(at_idx + slash_idx);
			password = username + "h360";
			LOG_INFO << "Username: " << username;
			LOG_INFO << "Password: " << password;
			std::string uri = "sftp://" + username + ":" + password + ip + "/" + uriToUpload;
			LOG_INFO << "Uploading on: " << uri;

			CURL* curlHandle = nullptr;
			curl_global_init(CURL_GLOBAL_ALL);
			curlHandle = curl_easy_init();
			std::string filepath = m_Impl->m_MentorLayerFilepath + ".mp4";
			if (!m_Impl->SFTPResumeUpload(curlHandle, uri.c_str(), filepath.c_str()))
			{
				LOG_ERROR << "Failed to upload Mentor Layer to Omniconnect";
			}
			curl_easy_cleanup(curlHandle);
			curl_global_cleanup();
		}
		else
		{
			LOG_ERROR << "The username from Omniconnect server is not correct.";
		}
	});

	/// Settings Loaded
	m_Impl->m_Settings->OnSettingsLoaded()
		.subscribe([this](auto _)
	{
		/// load the json file
		if ((!m_Impl->m_Settings->VideoListResponseFilepath.empty()) && (m_Impl->m_Settings->SelectedVideoListIndex != -1))
		{
			std::ifstream inFd(m_Impl->m_Settings->VideoListResponseFilepath);
			if (!inFd.is_open())
			{
				LOG_ERROR << "Failed to read file: " << m_Impl->m_Settings->VideoListResponseFilepath;
			}
			rapidjson::IStreamWrapper isw(inFd);
			rapidjson::Document d;
			d.ParseStream(isw);
			if (!d.HasMember("message"))
			{
				LOG_ERROR << "/GetVideoListsToEnrich response is corrupted.";
				return;
			}
			m_Impl->m_VideoList = std::make_shared<std::vector<OmniconnectVideoElement>>();
			auto msgArray = d["message"].GetArray();
			for (const auto& elmnt : msgArray)
			{
				auto values = elmnt.GetArray();
				OmniconnectVideoElement videoElmnt;
				videoElmnt.Uri = values[0].GetString();
				videoElmnt.Name = values[1].GetString();
				videoElmnt.DateAdded = values[2].GetString();
				videoElmnt.UploadUser = values[3].GetString();
				videoElmnt.UploadUri = values[4].GetString();
				m_Impl->m_VideoList->emplace_back(videoElmnt);
			}
			inFd.close();
		}
	});
}

rxcpp::observer<OmniconnectModel::prj_set_ptr_t> OmniconnectModel::ProjectSettingsFlowIn()
{
	return m_Impl->m_ProjectSettingsFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<void*> OmniconnectModel::ShowMediaListFlowIn()
{
	return m_Impl->m_ShowMediaListFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<int> OmniconnectModel::VideoListIndexFlowIn()
{
	return m_Impl->m_VideoListIndexFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observable<std::string> OmniconnectModel::VideoFilepathFlowOut()
{
	return m_Impl->m_VideoFilepathFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<omni_video_list_ptr_t> OmniconnectModel::VideoListFlowOut()
{
	return m_Impl->m_VideoListsFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observable<float> OmniconnectModel::ProgressFlowOut()
{
	return m_Impl->m_ProgressFlowOutSubj.get_observable().as_dynamic();
}
rxcpp::observer<void*> OmniconnectModel::UploadMentorLayerFlowIn()
{
	return m_Impl->m_UploadMentorVideoFileFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observer<void*> OmniconnectModel::ExportMentorFileFlowIn()
{
	return m_Impl->m_ExportMentorFileFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
rxcpp::observable<std::string> OmniconnectModel::ExportMentorLayerFilepathFlowOut()
{
	return m_Impl->m_ExpotMentorLayerFilepathFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu