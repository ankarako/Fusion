#include <Initializable.h>
#include <Destroyable.h>
#include <Core/Coordination.h>
#include <Core/UIRxDispatcher.h>

#include <Models/LoggerModel.h>
#include <Models/PerformanceImportModel.h>
#include <Models/MultiViewPlayerModel.h>
#include <Models/MVTTexturingModel.h>

#include <Presenters/MultiViewPlayerPresenter.h>
#include <Presenters/PerformanceImportPresenter.h>
#include <Presenters/MVTTexturingPresenter.h>

#include <CLI11.hpp>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Log.h>

#include <chrono>
#include <iostream>

///	\class Progress
///	\brief prints a console progress bar
class Progress
{
public:
	/// Construction
	///
	Progress(unsigned int total, unsigned int width, char complete, char incomplete)
		: m_TotalTicks(total)
		, m_BarWidth(width)
		, m_CompleteChar(complete)
		, m_IncompleteChar(incomplete)
	{ }
	///
	Progress(unsigned int total, unsigned int width)
		: m_TotalTicks(total)
		, m_BarWidth(width)
	{ }

	unsigned int operator++() { return ++m_Ticks; }

	void Display() const
	{
		float prog = (float)m_Ticks / m_TotalTicks;
		int pos = (int)(m_BarWidth * prog);
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_StartTime).count();
		
		std::cout << "[";
		
		for (int i = 0; i < m_BarWidth; ++i)
		{
			if (i < pos)		std::cout << m_CompleteChar;
			else if (i == pos)	std::cout << ">";
			else				std::cout << m_IncompleteChar;
		}
		std::cout << "] " << int(prog * 100.0) << "% "
			<< float(time_elapsed) / 1000.0 << "s\r";
		std::cout.flush();
	}

	void Done()
	{
		Display();
		std::cout << std::endl;
	}
private:
	unsigned int		m_Ticks = 0;
	const unsigned int	m_TotalTicks;
	const unsigned int	m_BarWidth;
	const char m_CompleteChar	= '=';
	const char m_IncompleteChar = ' ';
	const std::chrono::steady_clock::time_point m_StartTime = std::chrono::steady_clock::now();
};

using prog_ptr_t = std::shared_ptr<Progress>;
///=================
/// input arguments
///=================
std::string arg_InputFile;
std::string arg_OutputDir;
unsigned int	arg_TextureWidth	= 2048;
unsigned int	arg_TextureHeight	= 2048;
unsigned int	arg_SubSampleFactor = 4;
bool			arg_Merge			= false;
bool			arg_Blend			= false;
bool			arg_Undistort		= false;
bool			arg_SaveExtraData	= false;
///	Typedefs
/// models
using mvt_model_ptr_t = std::shared_ptr<fu::mvt::MVTModel>;
using mvp_model_ptr_t = std::shared_ptr<fu::mvt::MultiViewPlayerModel>;
using perfimport_model_ptr_t = std::shared_ptr<fu::mvt::PerformanceImportModel>;
/// presenters
using mvt_presenter_ptr_t = std::shared_ptr<fu::mvt::MVTPresenter>;
using mvp_presenter_ptr_t = std::shared_ptr<fu::mvt::MultiViewPlayerPresenter>;
using perfimport_presenter_ptr_t = std::shared_ptr<fu::mvt::PerformanceImportPresenter>;

int main(int argc, char* argv[])
{
	CLI::App app{ "RayMVT - Multi-View Texturing via RayCasting" };
	app.add_option("-i,--input_file",		arg_InputFile,			"The input .perf file.");
	app.add_option("-o,--output_dir",		arg_OutputDir,			"The output directory (where the computed texture maps will be saved).");
	app.add_option("--tex_width",			arg_TextureWidth,		"The output texture map width (default=2048).");
	app.add_option("--tex_height",			arg_TextureHeight,		"The output texture map height (default=2048).");
	app.add_option("-s,--subsample_factor", arg_SubSampleFactor,	"The ray casting subsampling factor (default=4).");
	app.add_flag("-m,--merge",				arg_Merge,				"use this flag to merge the textures from every view");
	app.add_flag("-b,--blend",				arg_Blend,				"use this flag together with <--merge> to blend the merged texture map (not implemented at the moment)");
	app.add_flag("-u,--undistort",			arg_Undistort,			"use this flag to undistort the viewpoint images");
	app.add_flag("-e,--extra_data",			arg_SaveExtraData,		"use this flag to save extra data (distorted & undistorted viewpoint textures for every frame).");

	/// create out models
	mvt_model_ptr_t mvt_model = std::make_shared<fu::mvt::MVTModel>();
	mvp_model_ptr_t mvp_model = std::make_shared<fu::mvt::MultiViewPlayerModel>();
	perfimport_model_ptr_t perfimport_model = std::make_shared<fu::mvt::PerformanceImportModel>();
	mvt_presenter_ptr_t mvt_presenter = std::make_shared<fu::mvt::MVTPresenter>(mvt_model, perfimport_model, mvp_model);
	mvp_presenter_ptr_t mvp_presenter = std::make_shared<fu::mvt::MultiViewPlayerPresenter>(mvp_model, perfimport_model);
	prog_ptr_t prog;

	CLI11_PARSE(app, argc, argv);
	plog::ColorConsoleAppender<plog::TxtFormatter> consolelogger;
#if defined(_DEBUG)
	plog::init(plog::debug, &consolelogger);
#else
	plog::init(plog::error, &consolelogger);
#endif
	/// initialize the everythin
	mvp_model->Init();
	mvt_model->Init();
	perfimport_model->Init();
	mvp_presenter->Init();
	mvt_presenter->Init();

	mvt_model->FrameCountFlowOut()
		.subscribe([&prog](unsigned int count) 
	{
		prog = std::make_shared<Progress>(count, 50);
		prog->Display();
	});

	mvt_model->ProgressTickFlowOut()
		.subscribe([&prog](auto _) 
	{
		prog->operator++();
		prog->Display();
	});
	/// set arguments
	/// mvp model
	mvp_model->SetDebugFramesEnabled(arg_SaveExtraData);
	mvp_model->SetUndistortEnabled(arg_Undistort);
	/// mvt model
	mvt_model->SetOutputDir(arg_OutputDir);
	mvt_model->SetExportDir(arg_OutputDir);
	mvp_model->SetDebugOutDir(arg_OutputDir + "\\extra");
	mvt_model->SetExtraDataDir(arg_OutputDir + "\\extra");
	mvt_model->SetTextureSize(make_uint2(arg_TextureWidth, arg_TextureHeight));
	mvt_model->SetSeparateTextures(!arg_Merge);
	mvt_model->SetLaunchMult(arg_SubSampleFactor);
	mvt_model->SetViewportEnabled(false);
	mvt_model->SetSaveAnimatedMeshEnabled(arg_SaveExtraData);
	mvt_model->SetTempFolderPath("temp");
	mvt_model->SetSkeletonFilename(perfimport_model->SkeletonFilename());
	mvt_model->SetSkinningFilename(perfimport_model->SkinningFilename());
	mvt_model->SetTemplateMeshFilename(perfimport_model->TemplateMeshFilename());
	mvt_model->SetTrackedParamsFilename(perfimport_model->TrackedParamsFilename());
	
	/// begin loop
	perfimport_model->ImportFilepathFlowIn().on_next(arg_InputFile);
	/// destroy everything when done
	mvt_model->Destroy();
	mvp_model->Destroy();
	perfimport_model->Destroy();
	return 0;
}