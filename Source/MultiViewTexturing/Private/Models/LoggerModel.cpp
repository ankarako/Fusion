#include <Models/LoggerModel.h>

#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Log.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief LoggerModel Implementation
struct LoggerModel::Impl
{
	using console_app_t = std::shared_ptr<plog::ColorConsoleAppender<plog::TxtFormatter>>;
	using file_app_t = std::shared_ptr<plog::RollingFileAppender<plog::TxtFormatter>>;
	static constexpr const char* k_LogFilename = "Log.txt";

	console_app_t 	m_ConsoleAppender;
	file_app_t 		m_FileAppender;
	/// Construction
	Impl() { }
};	///	!struct Impl
/// Construction
LoggerModel::LoggerModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
///	\brief initialize the model
void LoggerModel::Init()
{
	m_Impl->m_ConsoleAppender = std::make_shared<plog::ColorConsoleAppender<plog::TxtFormatter>>();
	m_Impl->m_FileAppender = std::make_shared<plog::RollingFileAppender<plog::TxtFormatter>>(m_Impl->k_LogFilename);
	plog::init(plog::debug, m_Impl->m_ConsoleAppender.get()).addAppender(m_Impl->m_FileAppender.get());
}
}	///	!namespace mvt
}	///	!namespace fu