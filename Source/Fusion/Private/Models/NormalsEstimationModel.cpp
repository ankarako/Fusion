#include <Models/NormalsEstimationModel.h>
#include <Models/ProjectModel.h>
#include <Core/Coordination.h>
#include <Core/SettingsRepo.h>
#include <Settings/NormalEstimationSettings.h>
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
	using settings_ptr_t = std::shared_ptr<NormalEstimationSettings>;

	static constexpr const char* k_PythonInferScriptPath	= "Resources/Python/HSSR/infer.py";
	static constexpr const char* k_ModelWeightPath			= "Resources/Python/HSSR/weights/vgg16_unet_quaternion_smoothness_model_e50.chkp";
	prj_model_ptr_t m_ProjectModel;
	coord_ptr_t		m_Coordination;
	srepo_ptr_t		m_SettingsRepo;
	settings_ptr_t	m_Settings;
	rxcpp::subjects::subject<std::pair<uint2, BufferCPU<uchar4>>>	m_FrameFlowInSubj;
	rxcpp::subjects::subject<BufferCPU<uchar4>>						m_NormalsBufferRGBAFlowOutSubj;
	rxcpp::subjects::subject<BufferCPU<float3>>						m_NormalsBufferFloatFlowOutSubj;

	Impl(prj_model_ptr_t prj_model, coord_ptr_t coord, srepo_ptr_t srepo)
		: m_ProjectModel(prj_model), m_Coordination(coord), m_SettingsRepo(srepo), m_Settings(std::make_shared<NormalEstimationSettings>())
	{ }
};	///	!struct Impl
/// Construction
NormalsEstimationModel::NormalsEstimationModel(prj_model_ptr_t prj_model, coord_ptr_t coord, srepo_ptr_t srepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(prj_model, coord, srepo))
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
		m_Impl->m_Settings->NormalFilepathEXR = outfpath_exr;
		m_Impl->m_Settings->NormalFilepathPNG = outfpath_png;
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
		/// load png
		cv::Mat normals_rgba = cv::Mat::zeros(256, 512, CV_8UC4);
		normals_rgba = cv::imread(outfpath_png, cv::IMREAD_ANYCOLOR);
		cv::cvtColor(normals_rgba, normals_rgba, cv::COLOR_BGR2RGBA);
		/// create buffer
		BufferCPU<uchar4> normalsbuf_rgba = CreateBufferCPU<uchar4>(256 * 512);
		std::memcpy(normalsbuf_rgba->Data(), normals_rgba.data, normalsbuf_rgba->ByteSize());
		/// send the buffer
		m_Impl->m_NormalsBufferRGBAFlowOutSubj.get_subscriber().on_next(normalsbuf_rgba);
		/// load exr
		cv::Mat normals_exr = cv::Mat::zeros(256, 512, CV_32FC3);
		normals_exr = cv::imread(outfpath_exr, cv::IMREAD_UNCHANGED);
		BufferCPU<float3> normalsbuf_float = CreateBufferCPU<float3>(256 * 512);
		std::memcpy(normalsbuf_float->Data(), normals_exr.data, normalsbuf_float->ByteSize());
		m_Impl->m_NormalsBufferFloatFlowOutSubj.get_subscriber().on_next(normalsbuf_float);
	});
	///=======================
	/// Settings Loaded Task
	///=======================
	m_Impl->m_Settings->OnSettingsLoaded()
		.subscribe([this](auto _) 
	{
		/// load png
		cv::Mat normals_rgba = cv::Mat::zeros(256, 512, CV_8UC4);
		LOG_DEBUG << "Loaded PNG: " << m_Impl->m_Settings->NormalFilepathPNG;
		normals_rgba = cv::imread(m_Impl->m_Settings->NormalFilepathPNG, cv::IMREAD_ANYCOLOR);
		cv::cvtColor(normals_rgba, normals_rgba, cv::COLOR_BGR2RGBA);
		/// create buffer
		BufferCPU<uchar4> normalsbuf_rgba = CreateBufferCPU<uchar4>(256 * 512);
		std::memcpy(normalsbuf_rgba->Data(), normals_rgba.data, normalsbuf_rgba->ByteSize());
		/// send the buffer
		m_Impl->m_NormalsBufferRGBAFlowOutSubj.get_subscriber().on_next(normalsbuf_rgba);
		/// load exr
		LOG_DEBUG << "Loaded EXR: " << m_Impl->m_Settings->NormalFilepathEXR;
		cv::Mat normals_exr = cv::Mat::zeros(256, 512, CV_32FC3);
		normals_exr = cv::imread(m_Impl->m_Settings->NormalFilepathEXR, cv::IMREAD_UNCHANGED);
		BufferCPU<float3> normalsbuf_float = CreateBufferCPU<float3>(256 * 512);
		std::memcpy(normalsbuf_float->Data(), normals_exr.data, normalsbuf_float->ByteSize());
		m_Impl->m_NormalsBufferFloatFlowOutSubj.get_subscriber().on_next(normalsbuf_float);
	}, [this](std::exception_ptr ptr) 
	{
		if (ptr)
		{
			try
			{
				std::rethrow_exception(ptr);
			}
			catch (std::exception & ex)
			{
				LOG_ERROR << ex.what();
			}
		}
	});
	///=======================
	/// Register our settings
	///=======================
	m_Impl->m_SettingsRepo->RegisterSettings(m_Impl->m_Settings);
}

rxcpp::observer<std::pair<uint2, BufferCPU<uchar4>>> fu::fusion::NormalsEstimationModel::FrameFlowIn()
{
	return m_Impl->m_FrameFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<BufferCPU<uchar4>> fu::fusion::NormalsEstimationModel::NormalsBufferRGBAFlowOut()
{
	return m_Impl->m_NormalsBufferRGBAFlowOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<BufferCPU<float3>> fu::fusion::NormalsEstimationModel::NormalsBufferFloatFlowOut()
{
	return m_Impl->m_NormalsBufferFloatFlowOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu