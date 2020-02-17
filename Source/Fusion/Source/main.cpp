#include <boost/di.hpp>
#include <Application.h>
#include <di/AppDi.h>

#include <plog/Log.h>

int main(int argc, char* argv[])
{
	try 
	{
		auto injector = boost::di::make_injector(fusion::di::AppDiModule());
		injector.create<app::Application>().Run();
	}
	catch (const std::exception& ex)
	{
		LOG_ERROR << "Failed to create application. Error: " << ex.what();
		return std::getchar();
	}
	return 0;
}