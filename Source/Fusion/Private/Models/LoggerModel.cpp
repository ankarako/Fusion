#include <Models/LoggerModel.h>
#include <Core/SettingsRepo.h>

#include <filesystem>

#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/TxtFormatter.h>

#include <iostream>
#include <sstream>

namespace fu {
namespace fusion {

///	\struct Impl
///	\brief logger model implementation
struct LoggerModel::Impl
{
	using console_app_t = std::shared_ptr<plog::ColorConsoleAppender<plog::TxtFormatter>>;
	using file_app_t = std::shared_ptr<plog::RollingFileAppender<plog::TxtFormatter>>;

	static constexpr const char* k_LogFilename = "FusionLog.txt";
	srepo_ptr_t	m_Srepo;
	
	console_app_t	m_ConsoleAppender;
	file_app_t		m_FileAppender;

	Impl(srepo_ptr_t srepo)
		: m_Srepo(srepo)
	{ }
};	///	!struct Impl

LoggerModel::LoggerModel(srepo_ptr_t srepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(srepo))
{ }

void LoggerModel::Init()
{
	m_Impl->m_ConsoleAppender = std::make_shared<plog::ColorConsoleAppender<plog::TxtFormatter>>();
	/// TODO: implement logger settings
	m_Impl->m_FileAppender = std::make_shared<plog::RollingFileAppender<plog::TxtFormatter>>(m_Impl->k_LogFilename);
#if defined(_DEBUG)
	plog::init(plog::debug, m_Impl->m_ConsoleAppender.get()).addAppender(m_Impl->m_FileAppender.get());
#else
	plog::init(plog::info, m_Impl->m_ConsoleAppender.get()).addAppender(m_Impl->m_FileAppender.get());
#endif	///	!_DEBUG
}
}	///	!namespace fusion
}	///	!namespace fu
