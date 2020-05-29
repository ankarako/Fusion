#ifndef __MVT_PUBLIC_DI_APPDI_H__
#define __MVT_PUBLIC_DI_APPDI_H__

#include <boost/di.hpp>
/// Interfaces
#include <Initializable.h>
#include <Updateable.h>
#include <Destroyable.h>
#include <Renderable.h>
#include <Widget.h>
#include <WindowFlags.h>
#include <FontManager.h>
#include <WidgetRepo.h>
#include <AppWindow.h>
#include <ImGuiWindow.h>
#include <InputManager.h>
/// Core
#include <Core/Coordination.h>
#include <Core/UIRxDispatcher.h>
/// Models
#include <Models/LoggerModel.h>
#include <Models/PerformanceImportModel.h>
#include <Models/FileExplorerModel.h>
#include <Models/MultiViewPlayerModel.h>
#include <Models/ViewportTracingModel.h>
#include <Models/MVTTexturingModel.h>
/// Presenters
#include <Presenters/MainToolbarPresenter.h>
#include <Presenters/FileMenuPresenter.h>
#include <Presenters/FileExplorerPresenter.h>
#include <Presenters/ViewportPresenter.h>
#include <Presenters/MultiViewPlayerPresenter.h>
#include <Presenters/PerformanceImportPresenter.h>
#include <Presenters/MVTTexturingPresenter.h>
/// Views
#include <Views/MainToolbarView.h>
#include <Views/FileExplorerView.h>
#include <Views/ViewportView.h>

namespace fu {
namespace mvt {
	/// Application di module
	auto AppDiModule = []() {
		/// injector
		auto Injector = boost::di::make_injector(
			boost::di::bind<app::ObsCoordination>().to<Coordination>().in(boost::di::singleton),
			///===============
			/// Initializables
			///===============
			boost::di::bind<app::Initializable *[]>().to<
				app::AppWindow,
				app::FontManager,
				LoggerModel,
				MainToolbarPresenter,
				FileMenuPresenter,
				PerformanceImportModel,
				FileExplorerView,
				FileExplorerPresenter,
				FileExplorerModel,
				ViewportTracingModel,	
				ViewportView,
				ViewportPresenter,
				MultiViewPlayerModel,
				MultiViewPlayerPresenter,
				PerformanceImportPresenter,
				MVTModel,
				MVTPresenter
			>(),
			///============
			/// Updateables
			///============
			boost::di::bind<app::Updateable *[]>().to<
				app::AppWindow,
				UIRxDispatcher
			>(),
			///=============
			/// Destroyables
			///=============
			boost::di::bind<app::Destroyable *[]>().to<
				app::AppWindow,
				FileExplorerModel,
				MultiViewPlayerModel,
				PerformanceImportModel,	
				ViewportTracingModel
			>(),
			///============
			///	Renderables
			///============
			boost::di::bind<app::Renderable>().to<
				app::WidgetRepo
			>(),
			///=========
			/// Widgets
			///=========
			boost::di::bind<app::Widget *[]>().to<
				MainToolbarView,
				FileExplorerView,
				ViewportView
			>()
		);
		return Injector;
	};
}	///	!namesapce mvt
}	/// !namespace fu
#endif	///	!__MVT_PUBLIC_DI_APPDI_H__