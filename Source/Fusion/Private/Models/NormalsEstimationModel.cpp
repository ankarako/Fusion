#include <Models/NormalsEstimationModel.h>
#include <Models/ProjectModel.h>
#include <Core/Coordination.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <filesystem>
#include <plog/Log.h>

namespace fu {
namespace fusion {

struct NormalsEstimationModel::Impl
{
	static constexpr const char* k_PythonInferScriptPath	= "Resources/Python/HSSR/infer.py";
	static constexpr const char* k_ModelWeightPath			= "Resources/Python/HSSR/weights/vgg16_unet_quaternion_smoothness_model_e50.chkp";
	prj_model_ptr_t m_ProjectModel;
	coord_ptr_t		m_Coordination;

	rxcpp::subjects::subject<std::pair<uint2, BufferCPU<uchar4>>>	m_FrameFlowInSubj;
	rxcpp::subjects::subject<std::string>							m_NormalsFilepathFlowOutExrSubj;
	rxcpp::subjects::subject<std::string>							m_NormalsFilepathFlowOutPngSubj;

	Impl(prj_model_ptr_t prj_model, coord_ptr_t coord)
		: m_ProjectModel(prj_model), m_Coordination(coord)
	{ }
};	///	!struct Impl
/// Construction
NormalsEstimationModel::NormalsEstimationModel(prj_model_ptr_t prj_model, coord_ptr_t coord)
	: m_Impl(spimpl::make_unique_impl<Impl>(prj_model, coord))
{ 

}

void NormalsEstimationModel::Init()
{
	///===================
	/// Frame flow in task
	///====================
	m_Impl->m_FrameFlowInSubj.get_observable().as_dynamic()
		/*.observe_on(m_Impl->m_Coordination->ModelCoordination())*/
		.subscribe([this](const std::pair<uint2, BufferCPU<uchar4>>& size_frame_pair)
	{
		uint2 size = std::get<0>(size_frame_pair);
		BufferCPU<uchar4> f = std::get<1>(size_frame_pair);
		/// create a mat from the frame
		cv::Mat mat = cv::Mat::zeros(size.y, size.x, CV_8UC4);
		std::memcpy(mat.data, f->Data(), f->ByteSize());
		/// resize the image and save
		cv::Mat tosave = cv::Mat::zeros(256, 512, CV_8UC4);
		cv::resize(mat, mat, cv::Size(512, 256));
		cv::cvtColor(mat, tosave, cv::COLOR_BGRA2RGB);
		std::string filepath = m_Impl->m_ProjectModel->WorkSpaceDirectory() + "/color_normal_est_input.png";
		std::string outfpath_exr = m_Impl->m_ProjectModel->WorkSpaceDirectory() + "/est_normals_output.exr";
		std::string outfpath_png = m_Impl->m_ProjectModel->WorkSpaceDirectory() + "/est_normals_output.png";
		cv::imwrite(filepath, tosave);
		/// make cli
		std::string absscriptpath = std::filesystem::absolute(std::filesystem::path(m_Impl->k_PythonInferScriptPath)).generic_string();
		std::string absinputfpath = std::filesystem::absolute(std::filesystem::path(filepath)).generic_string();
		std::string absweightpath = std::filesystem::absolute(std::filesystem::path(m_Impl->k_ModelWeightPath)).generic_string();
		std::string absoutputpath = std::filesystem::absolute(std::filesystem::path(outfpath_exr)).generic_string();
		std::string cli = std::string("python")
			+ " " + absscriptpath
			+ " " + "--input"
			+ " " + absinputfpath
			+ " " + "--output"
			+ " " + absoutputpath
			+ " " + "--weights"
			+ " " + absweightpath
			+ " " + "--png";
		LOG_DEBUG << "Running cli: " << cli;
		/// run normals estimation script
		std::system(cli.c_str());
		/// load the images to buffers and send them out
	});
}

rxcpp::observer<std::pair<uint2, BufferCPU<uchar4>>> fu::fusion::NormalsEstimationModel::FrameFlowIn()
{
	return m_Impl->m_FrameFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<std::string> fu::fusion::NormalsEstimationModel::NormalsFilepathFlowOutExr()
{
	return m_Impl->m_NormalsFilepathFlowOutExrSubj.get_observable().as_dynamic();
}

rxcpp::observable<std::string> fu::fusion::NormalsEstimationModel::NormalsFilePathFlowOutPng()
{
	return m_Impl->m_NormalsFilepathFlowOutPngSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu