#include <Models/IlluminationEstimationModel.h>
#include <Models/ProjectModel.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <filesystem>
#include <plog/Log.h>
namespace fu {
namespace fusion {

struct IlluminationEstimationModel::Impl
{
	static constexpr const char* k_PythonInferScriptPath = "Resources/Python/Illum/inference_mod.py";
	static constexpr const char* k_Ldr2ModelPath = "Resources/Python/Illum/ldr2hdr.pth";
	static constexpr const char* k_CheckpointPath = "Resources/Python/Illum/model.pth";

	prj_model_ptr_t		m_ProjectModel;

	rxcpp::subjects::subject<std::pair<uint2, BufferCPU<uchar4>>>	m_FrameFlowInSubj;
	rxcpp::subjects::subject<BufferCPU<uchar4>>						m_IlluminationMapFlowOutSubj;

	Impl(prj_model_ptr_t prj_model)
		: m_ProjectModel(prj_model)
	{ }
};
/// Constriction
IlluminationEstimationModel::IlluminationEstimationModel(prj_model_ptr_t prj_model)
	: m_Impl(spimpl::make_unique_impl<Impl>(prj_model))
{ }

void IlluminationEstimationModel::Init()
{
	m_Impl->m_FrameFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const std::pair<uint2, BufferCPU<uchar4>>& size_buf_pair) 
	{
		uint2 size = std::get<0>(size_buf_pair);
		BufferCPU<uchar4> f = std::get<1>(size_buf_pair);

		cv::Mat mat = cv::Mat::zeros(size.y, size.x, CV_8UC4);
		std::memcpy(mat.data, f->Data(), f->ByteSize());

		cv::Mat tosave = cv::Mat::zeros(256, 512, CV_8UC4);
		cv::resize(mat, mat, cv::Size(512, 256));
		cv::cvtColor(mat, tosave, cv::COLOR_BGRA2RGB);

		std::string filepath = m_Impl->m_ProjectModel->WorkSpaceDirectory() + "/color_illum_est_input.png";
		std::string outfpath_exr = m_Impl->m_ProjectModel->WorkSpaceDirectory() + "/est_illum_output";
		cv::imwrite(filepath, tosave);
		std::string absscriptpath = std::filesystem::absolute(std::filesystem::path(m_Impl->k_PythonInferScriptPath)).generic_string();
		std::string absinputfpath = std::filesystem::absolute(std::filesystem::path(filepath)).generic_string();
		std::string absoutputpath = std::filesystem::absolute(std::filesystem::path(outfpath_exr)).generic_string();
		std::string modelabsfpath = std::filesystem::absolute(std::filesystem::path(m_Impl->k_Ldr2ModelPath)).generic_string();
		std::string abschkpntpath = std::filesystem::absolute(std::filesystem::path(m_Impl->k_CheckpointPath)).generic_string();
		std::string cli = std::string("python")
			+ " " + absscriptpath
			+ " " + "--input_path"
			+ " " + absinputfpath
			+ " " + "--out_path"
			+ " " + absoutputpath
			+ " " + "--chkpnt_path"
			+ " " + abschkpntpath
			+ " " + "--ldr2hdr_model"
			+ " " + modelabsfpath;
		LOG_DEBUG << "Running cli: " << cli;
		std::system(cli.c_str());

		/// open result image
		cv::Mat illum_exr = cv::imread(absoutputpath + ".exr", cv::IMREAD_UNCHANGED);
		/// convert to ldr
		double max = -1e16;
		double min = 1e16;
		cv::minMaxLoc(illum_exr, &min, &max);
		cv::normalize(illum_exr, illum_exr, 0.0f, 1.0f, cv::NORM_MINMAX);
		cv::convertScaleAbs(illum_exr, illum_exr, 255.0, 0.0);
		cv::cvtColor(illum_exr, illum_exr, cv::COLOR_BGR2RGBA);

		BufferCPU<uchar4> frame = CreateBufferCPU<uchar4>(illum_exr.cols * illum_exr.rows);
		std::memcpy(frame->Data(), illum_exr.data, frame->ByteSize());
		m_Impl->m_IlluminationMapFlowOutSubj.get_subscriber().on_next(frame);
	});
}

rxcpp::observer<std::pair<uint2, BufferCPU<uchar4>>> fu::fusion::IlluminationEstimationModel::FrameFlowIn()
{
	return m_Impl->m_FrameFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<BufferCPU<uchar4>> fu::fusion::IlluminationEstimationModel::IlluminationMapFlowOut()
{
	return m_Impl->m_IlluminationMapFlowOutSubj.get_observable().as_dynamic();
}
}
}	///	!namespace fu