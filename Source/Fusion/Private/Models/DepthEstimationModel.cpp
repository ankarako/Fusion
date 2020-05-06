#include <Models/DepthEstimationModel.h>
#include <Models/ProjectModel.h>
#include <Core/Coordination.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <filesystem>
#include <plog/Log.h>
#include <Core/DepthColorToPCL.h>
#include <Core/SettingsRepo.h>
#include <Settings/DepthEstimationSettings.h>
namespace fu {
namespace fusion {
struct DepthEstimationModel::Impl
{
	using settings_ptr_t = std::shared_ptr<DepthEstimationSettings>;
	/// 
	static constexpr const char* k_CaffeExecutablePath	= "Resources/Caffe/bin/caffe.exe";
	static constexpr const char* k_CaffeCommand			= "infer";
	std::string m_ModelFilepath		= "Resources/Caffe/model/uresnet_deploy.ptt";
	std::string m_WeightsFilepath	= "Resources/Caffe/weights/uresnet_weights.h5";

	prj_model_ptr_t	m_ProjectModel;
	coord_ptr_t		m_Coord;
	srepo_ptr_t		m_SettingsRepo;
	settings_ptr_t	m_Settings;

	rxcpp::subjects::subject<std::pair<uint2, BufferCPU<uchar4>>>	m_FrameFlowInSubj;
	rxcpp::subjects::subject<std::string>							m_PointcloudFilepathFlowOut;
	/// Construction
	Impl(prj_model_ptr_t prj_model, coord_ptr_t coord, srepo_ptr_t srepo)
		: m_ProjectModel(prj_model)
		, m_Coord(coord)
		, m_SettingsRepo(srepo)
		, m_Settings(std::make_shared<DepthEstimationSettings>())
	{ }
};
/// Construction
DepthEstimationModel::DepthEstimationModel(prj_model_ptr_t prj_model, coord_ptr_t coord, srepo_ptr_t srepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(prj_model, coord, srepo))
{ }

void DepthEstimationModel::Init()
{
	m_Impl->m_FrameFlowInSubj.get_observable().as_dynamic()
		//.observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](const std::pair<uint2, BufferCPU<uchar4>>& frame)
	{
		uint2 size = std::get<0>(frame);
		BufferCPU<uchar4> f = std::get<1>(frame);
		/// create an mat of the frame
		cv::Mat mat = cv::Mat::zeros(size.y, size.x, CV_8UC4);
		std::memcpy(mat.data, f->Data(), f->ByteSize());
		/// rescale the frame to 512 x 256 (that our network supprts)
		cv::Mat tosave = cv::Mat::zeros(256, 512, CV_8UC3);
		cv::resize(mat, mat, cv::Size(512, 256));
		cv::cvtColor(mat, tosave, cv::COLOR_BGRA2RGB);
		/// save
		std::string filepath = m_Impl->m_ProjectModel->WorkSpaceDirectory() + "/color_depth_est_input.png";
		cv::imwrite(filepath, tosave);
		/// make cli
		std::string absexe = std::filesystem::absolute(std::filesystem::path(m_Impl->k_CaffeExecutablePath)).generic_string();
		std::string absinputimg = std::filesystem::absolute(std::filesystem::path(filepath)).generic_string();
		std::string absoutimg = std::filesystem::absolute(std::filesystem::path(m_Impl->m_ProjectModel->WorkSpaceDirectory() + "/depth_est_output")).generic_string();
		m_Impl->m_Settings->DepthFilepathEXR = absoutimg;
		std::string cli = absexe + " " + std::string(m_Impl->k_CaffeCommand)
			+ " " + "--model"
			+ " " + m_Impl->m_ModelFilepath
			+ " " + "--weights"
			+ " " + m_Impl->m_WeightsFilepath
			+ " " + "--input"
			+ " " + absinputimg
			+ " " + "--output"
			+ " " + absoutimg
			+ " " + "--gpu 0";
		LOG_DEBUG << "Running cli: " << cli;
		/// run depth estimation network
		std::system(cli.c_str());
		std::string absoutply = std::filesystem::absolute(std::filesystem::path(m_Impl->m_ProjectModel->WorkSpaceDirectory() + "/depth_est_pcl.ply")).generic_string();
		m_Impl->m_Settings->PointCloudFilepathPLY = absoutply;
		/// send filepath to pcl loader
		DepthColorToPCL(absinputimg, absoutimg + ".exr", 10.0f, absoutply);
		m_Impl->m_PointcloudFilepathFlowOut.get_subscriber().on_next(absoutply);
	});
	///=====================
	/// Settings loaded Task
	///=====================
	m_Impl->m_Settings->OnSettingsLoaded()
		.observe_on(m_Impl->m_Coord->ModelCoordination())
		.subscribe([this](auto _) 
	{
		/// we just want to load the point cloud
		m_Impl->m_PointcloudFilepathFlowOut.get_subscriber().on_next(m_Impl->m_Settings->PointCloudFilepathPLY);
	});
	///======================
	/// Register our settings
	///=======================
	m_Impl->m_SettingsRepo->RegisterSettings(m_Impl->m_Settings);
}

rxcpp::observer<std::pair<uint2, BufferCPU<uchar4>>> fu::fusion::DepthEstimationModel::FrameFlowIn()
{
	return m_Impl->m_FrameFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<std::string> fu::fusion::DepthEstimationModel::PointCloudFilepathFlowOut()
{
	return m_Impl->m_PointcloudFilepathFlowOut.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namesapce fu