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
				app::AppWindow
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
				app::AppWindow
			>(),
			///============
			///	Renderables
			///============
			boost::di::bind<app::Renderable>().to<
				app::WidgetRepo
			>()
		);
		return Injector;
	};
}	///	!namesapce mvt
}	/// !namespace fu
#endif	///	!__MVT_PUBLIC_DI_APPDI_H__