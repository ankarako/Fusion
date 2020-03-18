#ifndef	__FUSION_PUBLIC_DI_APPDI_H__
#define __FUSION_PUBLIC_DI_APPDI_H__

#include <boost/di.hpp>
/// Interfaces
#include <Initializable.h>
#include <Updateable.h>
#include <Destroyable.h>
#include <Widget.h>
#include <WindowFlags.h>
#include <FontManager.h>
/// Core
#include <Core/SettingsRepo.h>
#include <Core/Coordination.h>
#include <Core/UIRxDispatcher.h>
///	implementations
#include <AppWindow.h>
#include <ImGuiWindow.h>
#include <InputManager.h>
#include <Renderable.h>
#include <WidgetRepo.h>
/// Models
#include <Models/LoggerModel.h>
#include <Models/FileExplorerModel.h>
#include <Models/PlayerModel.h>
#include <Models/VideoTracingModel.h>
#include <Models/RayTracingModel.h>
#include <Models/ProjectModel.h>
#include <Models/DepthEstimationModel.h>
/// Presenters
#include <Presenters/PlayerControllerPresenter.h>
#include <Presenters/PlayerViewportPresenter.h>
#include <Presenters/FileExplorerPresenter.h>
#include <Presenters/MainToolbarPresenter.h>
#include <Presenters/RayTracingPresenter.h>
///	Widgets
#include <Views/PlayerControllerView.h>
#include <Views/PlayerViewportView.h>
#include <Views/FileExplorerView.h>
#include <Views/MainToolbarView.h>
#include <Views/RayTracingView.h>
#include <Views/DepthEstimationSettingsView.h>
#include <Views/RayTracingControlView.h>
/// Asset Loading System
#include <Systems/AssetLoadingSystem.h>

namespace fu {
namespace fusion {
namespace di {
	///	application di module
	auto AppDiModule = []() {
		/// injector
		auto Injector = boost::di::make_injector(
			boost::di::bind<app::ObsCoordination>().to<Coordination>().in(boost::di::singleton),
			boost::di::bind<app::Initializable * []>().to<
			app::AppWindow,
			app::FontManager,
			LoggerModel,
			SettingsRepo,
			FileExplorerView,
			FileExplorerPresenter,
			FileExplorerModel,
			MainToolbarPresenter,
			PlayerViewportView,
			PlayerModel,
			PlayerControllerPresenter,
			PlayerViewportPresenter,
			VideoTracingModel,
			RayTracingPresenter,
			RayTracingModel,
			RayTracingView,
			ProjectModel,
			DepthEstimationModel
			>(),
			boost::di::bind<app::Updateable * []>().to<
			app::AppWindow,
			UIRxDispatcher,
			RayTracingModel
			>(),
			boost::di::bind<app::Destroyable * []>().to<
			app::AppWindow,
			FileExplorerModel,
			PlayerModel,
			VideoTracingModel
			>(),
			boost::di::bind<app::Renderable>().to<
			app::WidgetRepo
			>(),
			boost::di::bind<app::Widget * []>().to<
			PlayerControllerView
			>(),
			boost::di::bind<Settings<WriterType::File>>().to<SettingsRepo>()
		);
		return Injector;
	};
}	///	!namespace di
}	///	!namespace fusion
}
#endif	///	!__FUSION_PUBLIC_DI_APPDI_H__


