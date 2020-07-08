#ifndef	__FUSION_PUBLIC_DI_APPDI_H__
#define __FUSION_PUBLIC_DI_APPDI_H__

//#define __has_builtin(...)	1
//#define BOOST_DI_CFG_CTOR_LIMIT_SIZE 12

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
#include <Models/NormalsEstimationModel.h>
#include <Models/AssetLoadingModel.h>
#include <Models/SequencerModel.h>
#include <Models/PerfcapPlayerModel.h>
#include <Models/IlluminationEstimationModel.h>
#include <Models/AnimationModel.h>
#include <Models/WindowsMenuModel.h>
#include <Models/FuImportModel.h>
#include <Models/FusedExportModel.h>
/// Presenters
#include <Presenters/PlayerControllerPresenter.h>
#include <Presenters/PlayerViewportPresenter.h>
#include <Presenters/FileExplorerPresenter.h>
#include <Presenters/MainToolbarPresenter.h>
#include <Presenters/RayTracingPresenter.h>
#include <Presenters/SequencerPresenter.h>
#include <Presenters/PerfcapPlayerPresenter.h>
#include <Presenters/FileMenuPresenter.h>
#include <Presenters/FiltersMenuPresenter.h>
#include <Presenters/NormalsResultPresenter.h>
#include <Presenters/WindowsMenuPresenter.h>
#include <Presenters/TransformationsPresenter.h>
#include <Presenters/IlluminationResultPresenter.h>
#include <Presenters/FuImportPresenter.h>
#include <Presenters/ProgressBarPresenter.h>
#include <Presenters/FusedExportPresenter.h>
///	Widgets
#include <Views/PlayerControllerView.h>
#include <Views/PlayerViewportView.h>
#include <Views/FileExplorerView.h>
#include <Views/MainToolbarView.h>
#include <Views/RayTracingView.h>
#include <Views/DepthEstimationSettingsView.h>
#include <Views/RayTracingControlView.h>
#include <Views/SequencerView.h>
#include <Views/NormalsResultView.h>
#include <Views/TransformationView.h>
#include <Views/IlluminationResultView.h>
#include <Views/ProgressBarView.h>

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
			PlayerViewportView,
			PlayerModel,
			PlayerControllerPresenter,
			PlayerViewportPresenter,
			VideoTracingModel,
			RayTracingPresenter,
			RayTracingModel,
			RayTracingView,
			ProjectModel,
			DepthEstimationModel,
			NormalsEstimationModel,
			SequencerModel,
			SequencerPresenter,
			PerfcapPlayerPresenter,
			FileMenuPresenter,
			FiltersMenuPresenter,
			MainToolbarPresenter,
			NormalsResultView,
			NormalsResultPresenter,
			TransformationsPresenter,
			IlluminationEstimationModel,
			IlluminationResultView,
			IlluminationResultPresenter,
			AnimationModel,
			WindowsMenuModel,
			WindowsMenuPresenter,
			FuImportModel,
			FuImportPresenter,
			ProgressBarPresenter,
			FusedExportModel,
			FusedExportPresenter
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
			PlayerControllerView,
			PlayerViewportView,
			FileExplorerView,
			MainToolbarView,
			RayTracingView,
			DepthEstimationSettingsView,
			RayTracingControlView,
			SequencerView,
			NormalsResultView,
			TransformationView,
			IlluminationResultView,
			ProgressBarView
			>(),
			boost::di::bind<Settings<WriterType::File>>().to<SettingsRepo>()
		);
		return Injector;
	};
}	///	!namespace di
}	///	!namespace fusion
}
#endif	///	!__FUSION_PUBLIC_DI_APPDI_H__


