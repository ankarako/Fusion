#include <Models/FusedExportModel.h>
#include <TextureExportingNode.h>
#include <filesystem>
#include <plog/Log.h>
#include <sstream>

namespace fu {
namespace fusion {
struct FusedExportModel::Impl
{
	static constexpr const char* k_FfmpegPath = "Resources\\ffmpeg\\ffmpeg.exe";
	int											m_AnimatedFramesCount;
	int											m_CurrentExportingFrame = -1;
	uint2										m_ExportSize;
	std::string									m_ExportTempFolder;
	std::vector<SequenceItem>					m_SequenceItems;
	trans::TextureExportingNode					m_FrameEncodingNode;
	rxcpp::subjects::subject<int>				m_SeekFrameIdFlowOutSubj;
	rxcpp::subjects::subject<BufferCPU<uchar4>> m_FusedFrameFlowInSubj;
	rxcpp::subjects::subject<int>				m_AnitmatedFramesCountFlowInSubj;
	rxcpp::subjects::subject<std::string>		m_ExportFilepathFlowInSubj;
	rxcpp::subjects::subject<std::string>		m_ExportMentorLayerFilepathFlowInSubj;
	rxcpp::subjects::subject<uint2>				m_VideoSizeFlowInSubj;
	rxcpp::subjects::subject<void*>				m_StartedExportingFlowOutSubj;
	rxcpp::subjects::subject<void*>				m_FinishedExportingFlowOutSubj;
	rxcpp::subjects::subject<void*>				m_StartedExportingMentorLayerSubj;
	rxcpp::subjects::subject<void*>				m_FinishedExportingMentorLayerSubj;
	rxcpp::subjects::subject<SequenceItem>		m_SequenceItemFlowInSubj;

	Impl() 
		: m_FrameEncodingNode(trans::CreateTextureExportingNode())
	{ }
};	///	!struct Impl

FusedExportModel::FusedExportModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }

void FusedExportModel::Init()
{
	m_Impl->m_AnitmatedFramesCountFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](int frameCount) 
	{
		m_Impl->m_AnimatedFramesCount = frameCount;
	});

	m_Impl->m_VideoSizeFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const uint2& size) 
	{
		m_Impl->m_ExportSize = size;
	});

	m_Impl->m_SequenceItemFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const SequenceItem& item) 
	{
		m_Impl->m_SequenceItems.emplace_back(item);
	});
	///===========================
	/// Fused Video Exporting Task
	///===========================
	m_Impl->m_ExportFilepathFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::string& filepath) 
	{
		namespace fs = std::filesystem;
		std::string saveDir = fs::path(filepath).parent_path().generic_string();
		std::string videoFname = fs::path(filepath).filename().generic_string() + ".mp4";
		/// check if the filepath already exists
		if (!fs::exists(filepath))
		{
			m_Impl->m_StartedExportingFlowOutSubj.get_subscriber().on_next(nullptr);
			if (!fs::is_directory(filepath))
			{
				/// make a directory with the same name
				m_Impl->m_ExportTempFolder = fs::path(filepath).replace_extension("").generic_string();
			}
			m_Impl->m_ExportTempFolder = fs::path(filepath).replace_extension("").generic_string();
			/// create temporaty folder to save
			if (!fs::create_directory(m_Impl->m_ExportTempFolder))
			{
				LOG_ERROR << "Failed to create folder: " << m_Impl->m_ExportTempFolder;
			}
			/// now trigger the players
			/// exporting loop
			if (m_Impl->m_AnimatedFramesCount > 0)
			{
				for (size_t f = 0; f < m_Impl->m_AnimatedFramesCount; f++)
				{
					m_Impl->m_CurrentExportingFrame = f;
					std::this_thread::sleep_for(std::chrono::milliseconds(30));
					m_Impl->m_SeekFrameIdFlowOutSubj.get_subscriber().on_next(f);
				}
				/// when finished run ffmpeg to convert frames to video
				std::string cli = std::string(m_Impl->k_FfmpegPath) + " -r 29.99 -i " + m_Impl->m_ExportTempFolder + "\\" + "frame_%04d.png -vcodec libx264 -crf 10 -pix_fmt yuv420p " + saveDir + "\\" + videoFname;
				LOG_DEBUG << "Running: " << cli;
				std::system(cli.c_str());
				m_Impl->m_FinishedExportingFlowOutSubj.get_subscriber().on_next(nullptr);
				/// cleanup
				for (auto& entry : fs::recursive_directory_iterator(m_Impl->m_ExportTempFolder))
				{
					try
					{
						fs::remove(entry);
					}
					catch (std::exception & ex)
					{
						LOG_ERROR << "Failed to delete file: " << ex.what();
					}
				}
				try
				{
					fs::remove(m_Impl->m_ExportTempFolder);
				}
				catch (std::exception & ex)
				{
					LOG_ERROR << "Failed to delete file: " << ex.what();
				}
			}
			else
			{
				LOG_ERROR << "There are no animated frames to export.";
			}
		}
	});
	///==========================
	/// Mentor Layer Export Task
	///==========================
	m_Impl->m_ExportMentorLayerFilepathFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::string& filepath) 
	{
		namespace fs = std::filesystem;
		std::string saveDir = fs::path(filepath).parent_path().generic_string();
		std::string videoFname = fs::path(filepath).filename().generic_string() + ".mp4";
		/// check if the filepath already exists
		if (!fs::exists(filepath))
		{
			m_Impl->m_StartedExportingMentorLayerSubj.get_subscriber().on_next(nullptr);
			if (!fs::is_directory(filepath))
			{
				/// make a directory with the same name
				m_Impl->m_ExportTempFolder = fs::path(filepath).replace_extension("").generic_string();
			}
			m_Impl->m_ExportTempFolder = fs::path(filepath).replace_extension("").generic_string();
			/// create temporaty folder to save
			if (!fs::create_directory(m_Impl->m_ExportTempFolder))
			{
				LOG_ERROR << "Failed to create folder: " << m_Impl->m_ExportTempFolder;
			}
			/// now trigger the players
			/// exporting loop
			if (m_Impl->m_AnimatedFramesCount > 0)
			{
				for (size_t f = 0; f < m_Impl->m_AnimatedFramesCount; f++)
				{
					m_Impl->m_CurrentExportingFrame = f;
					std::this_thread::sleep_for(std::chrono::milliseconds(30));
					m_Impl->m_SeekFrameIdFlowOutSubj.get_subscriber().on_next(f);
				}
				/// when finished run ffmpeg to convert frames to video
				std::string cli = std::string(m_Impl->k_FfmpegPath) + " -r 29.99 -i " + m_Impl->m_ExportTempFolder + "\\" + "frame_%04d.png -vcodec libx264 -crf 10 -pix_fmt yuv420p " + saveDir + "\\" + videoFname;
				LOG_DEBUG << "Running: " << cli;
				std::system(cli.c_str());
				m_Impl->m_FinishedExportingMentorLayerSubj.get_subscriber().on_next(nullptr);
				/// cleanup
				for (auto& entry : fs::recursive_directory_iterator(m_Impl->m_ExportTempFolder))
				{
					try
					{
						fs::remove(entry);
					}
					catch (std::exception & ex)
					{
						LOG_ERROR << "Failed to delete file: " << ex.what();
					}
				}
				try
				{
					fs::remove(m_Impl->m_ExportTempFolder);
				}
				catch (std::exception & ex)
				{
					LOG_ERROR << "Failed to delete file: " << ex.what();
				}
			}
			else
			{
				LOG_ERROR << "There are no animated frames to export.";
			}
		}
	});
	/// Saving frames Tasks
	m_Impl->m_FusedFrameFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const BufferCPU<uchar4>& frame) 
	{
		std::stringstream ss;
		ss << "frame_" << std::setw(4) << std::setfill('0') << m_Impl->m_CurrentExportingFrame << ".png";
		std::string filepath = m_Impl->m_ExportTempFolder + "\\" + ss.str();
		m_Impl->m_FrameEncodingNode->ExportTexture(filepath, frame, m_Impl->m_ExportSize);
	});

}

rxcpp::observable<int> FusedExportModel::SeekFrameIdFlowOut()
{
	return m_Impl->m_SeekFrameIdFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observer<BufferCPU<uchar4>> FusedExportModel::FusedFrameFlowIn()
{
	return m_Impl->m_FusedFrameFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<int> FusedExportModel::AnimatedFramesCountFlowIn()
{
	return m_Impl->m_AnitmatedFramesCountFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<std::string> FusedExportModel::ExportFilepathFlowIn()
{
	return m_Impl->m_ExportFilepathFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<std::string> FusedExportModel::ExportMentorLayerFilepathFlowIn()
{
	return m_Impl->m_ExportMentorLayerFilepathFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<uint2> FusedExportModel::VideoSizeFlowIn()
{
	return m_Impl->m_VideoSizeFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<void*> FusedExportModel::StartedExportingFlowOut()
{
	return m_Impl->m_StartedExportingFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> FusedExportModel::StartedExportingMentorLayer()
{
	return m_Impl->m_StartedExportingMentorLayerSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> FusedExportModel::FinishedExportingFlowOut()
{
	return m_Impl->m_FinishedExportingFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<void*> FusedExportModel::FinishedExportingMentorLayerFlowOut()
{
	return m_Impl->m_FinishedExportingMentorLayerSubj.get_observable().as_dynamic();
}

rxcpp::observer<SequenceItem> FusedExportModel::SequenceItemFlowIn()
{
	return m_Impl->m_SequenceItemFlowInSubj.get_subscriber().get_observer().as_dynamic();
}


}	///	!namespace fusion
}	///	!namespace fu