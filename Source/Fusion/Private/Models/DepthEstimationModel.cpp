#include <Models/DepthEstimationModel.h>
#include <Models/ProjectModel.h>
#include <Core/Coordination.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>


namespace fu {
namespace fusion {
struct DepthEstimationModel::Impl
{
	/// 
	static constexpr const char* k_CaffeExecutablePath	= "Resources/Caffe/bin/caffe.exe";
	static constexpr const char* k_CaffeCommand			= "infer";
	std::string m_ModelFilepath		= "Resources/Caffe/model/uresnet_deploy.ptt";
	std::string m_WeightsFilepath	= "Resources/Caffe/weights/uresnet_weights.h5";

	prj_model_ptr_t	m_ProjectModel;
	coord_ptr_t		m_Coord;

	rxcpp::subjects::subject<std::pair<uint2, BufferCPU<uchar4>>>	m_FrameFlowInSubj;
	rxcpp::subjects::subject<std::string>							m_PointcloudFilepathFlowOut;
	/// Construction
	Impl(prj_model_ptr_t prj_model, coord_ptr_t coord)
		: m_ProjectModel(prj_model)
		, m_Coord(coord)
	{ }
};
/// Construction
DepthEstimationModel::DepthEstimationModel(prj_model_ptr_t prj_model, coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(prj_model, coord))
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
		cv::resize(mat, mat, cv::Size(512, 256));
		/// save
		std::string filepath = m_Impl->m_ProjectModel->WorkSpaceDirectory() + "/color_depth_est_input.png";
		cv::imwrite(filepath, mat);
		/// make cli
		std::string cli = std::string(m_Impl->k_CaffeExecutablePath) + " " + std::string(m_Impl->k_CaffeCommand)
			+ " " + "--model"
			+ " " + m_Impl->m_ModelFilepath
			+ " " + "--weights"
			+ " " + m_Impl->m_WeightsFilepath
			+ " " + "--input"
			+ " " + m_Impl->m_ProjectModel->WorkSpaceDirectory() + "/depth_est_input.png"
			+ " " + "--output"
			+ " " + m_Impl->m_ProjectModel->WorkSpaceDirectory() + "/depth_est_output";
		/// run depth estimation network
		std::system(cli.c_str());
		/// send filepath to pcl loader
		
	});
}

rxcpp::observer<std::pair<uint2, BufferCPU<uchar4>>> fu::fusion::DepthEstimationModel::FrameFlowIn()
{
	return m_Impl->m_FrameFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
}	///	!namespace fusion
}	///	!namesapce fu