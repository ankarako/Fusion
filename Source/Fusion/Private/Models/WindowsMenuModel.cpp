#include <Models/WindowsMenuModel.h>
#include <Core/SettingsRepo.h>
#include <Settings/WindowsMenuSettings.h>

namespace fu {
namespace fusion {
/// \struct Impl
///	\brief WindowsMenuModel Implementation
struct WindowsMenuModel::Impl
{
	/// Typedefs
	using settings_ptr_t = std::shared_ptr<WindowsMenuSettings>;
	/// State
	srepo_ptr_t		m_SettingsRepo;
	settings_ptr_t	m_Settings;
	/// Inputys
	rxcpp::subjects::subject<bool> m_NormalsWindowClickedFlowInSubj;
	rxcpp::subjects::subject<bool> m_IlluminationWindowClickedFlowInSubj;
	rxcpp::subjects::subject<bool> m_ViewportOptionsWindowClickedFlowInSubj;

	rxcpp::subjects::subject<bool> m_NormalsWindowVisibleOutSubj;
	rxcpp::subjects::subject<bool> m_IlluminationWindowVisibleOutSubj;
	rxcpp::subjects::subject<bool> m_ViewportOptionsWindowVisibleOutSubj;
	/// Contruction
	Impl(srepo_ptr_t srepo)
		: m_SettingsRepo(srepo)
		, m_Settings(std::make_shared<WindowsMenuSettings>())
	{ }
};	///	!struct Impl
/// Construction
WindowsMenuModel::WindowsMenuModel(srepo_ptr_t srepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(srepo))
{ }
///	\brief Initialize the model
void WindowsMenuModel::Init()
{
	///============================
	/// Normals Window clicked Task
	///============================
	m_Impl->m_NormalsWindowClickedFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](bool visible) 
	{
		m_Impl->m_Settings->NormalsWindowVisible = visible;
	});
	///==================================
	/// Illumination Window clicked Task
	///==================================
	m_Impl->m_IlluminationWindowClickedFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](bool visible) 
	{
		m_Impl->m_Settings->IlluminationWindowVisible = visible;
	});
	///====================================
	/// Viewport Options Window cliked Task
	///====================================
	m_Impl->m_ViewportOptionsWindowClickedFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](bool visible) 
	{
		m_Impl->m_Settings->ViewpotOptionsVisible = visible;
	});
	///======================
	/// Settings loaded Task
	///======================
	m_Impl->m_Settings->OnSettingsLoaded()
		.subscribe([this](auto _) 
	{
		m_Impl->m_NormalsWindowVisibleOutSubj.get_subscriber().on_next(m_Impl->m_Settings->NormalsWindowVisible);
		m_Impl->m_IlluminationWindowVisibleOutSubj.get_subscriber().on_next(m_Impl->m_Settings->IlluminationWindowVisible);
		m_Impl->m_ViewportOptionsWindowVisibleOutSubj.get_subscriber().on_next(m_Impl->m_Settings->ViewpotOptionsVisible);
	});
	///=======================
	/// Register our settings
	///=======================
	m_Impl->m_SettingsRepo->RegisterSettings(m_Impl->m_Settings);
}
/// Normal Window Clicked observer
rxcpp::observer<bool> WindowsMenuModel::NormalsWindowClickedFlowIn()
{
	return m_Impl->m_NormalsWindowClickedFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
/// Illumination WindowClicked observer
rxcpp::observer<bool> WindowsMenuModel::IlluminationWindowClickedFlowIn()
{
	return m_Impl->m_IlluminationWindowClickedFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
/// Viewport options Window clicked
rxcpp::observer<bool> WindowsMenuModel::ViewportOptionsWindowClickedFlowIn()
{
	return m_Impl->m_ViewportOptionsWindowClickedFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observable<bool> WindowsMenuModel::NormalsWindowVisibleFlowOut()
{
	return m_Impl->m_NormalsWindowVisibleOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<bool> WindowsMenuModel::IlluminationWindowVisibleFlowOut()
{
	return m_Impl->m_IlluminationWindowVisibleOutSubj.get_observable().as_dynamic();
}

rxcpp::observable<bool> WindowsMenuModel::ViewportOptionsWindowVisibleFlowOut()
{
	return m_Impl->m_ViewportOptionsWindowVisibleOutSubj.get_observable().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu