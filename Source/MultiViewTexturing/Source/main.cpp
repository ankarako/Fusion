#include <boost/di.hpp>
#include <Application.h>
#include <di/AppDi.h>

int main(int argc, char* argv[])
{
	try
	{
		auto injector = boost::di::make_injector(fu::mvt::AppDiModule());
		injector.create<fu::app::Application>().Run();
	}
	catch (const std::exception& ex)
	{
		std::cout << "Failed to create application. Error: " << ex.what();
		return std::getchar();
	}
	return 0;
}