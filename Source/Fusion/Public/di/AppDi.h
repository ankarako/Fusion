#ifndef	__FUSION_PUBLIC_DI_APPDI_H__
#define __FUSION_PUBLIC_DI_APPDI_H__

#include <boost/di.hpp>
/// Interfaces
#include <Initializable.h>
#include <Updateable.h>
#include <Destroyable.h>
#include <Widget.h>
#include <WindowFlags.h>>
/// Core
#include <Core/SettingsRepo.h>
///	implementations
#include <AppWindow.h>
#include <ImGuiWindow.h>
#include <InputManager.h>
#include <Renderable.h>
#include <WidgetRepo.h>

/// Models
#include <Models/LoggerModel.h>
#include <Models/PlayerModel.h>
/// Presenters
#include <Presenters/PlayerControllerPresenter.h>
///	Widgets
#include <Views/PlayerControllerView.h>

namespace fusion {
namespace di {
///	application di module
auto AppDiModule = []() {
	/// injector
	auto Injector = boost::di::make_injector(
		boost::di::bind<app::Initializable*[]>().to<
			app::AppWindow,
			LoggerModel,
			SettingsRepo,
			PlayerModel,
			PlayerControllerPresenter
		>(),
		boost::di::bind<app::Updateable*[]>().to<
			app::AppWindow
		>(),
		boost::di::bind<app::Destroyable*[]>().to<
			app::AppWindow
		>(),
		boost::di::bind<app::Renderable>().to<
			app::WidgetRepo
		>(),
		boost::di::bind<app::Widget*[]>().to<
			PlayerControllerView
		>(),
		boost::di::bind<Settings<WriterType::File>>().to<SettingsRepo>()
	);
	return Injector;
};
}	///	!namespace di
}	///	!namespace fusion
#endif	///	!__FUSION_PUBLIC_DI_APPDI_H__


