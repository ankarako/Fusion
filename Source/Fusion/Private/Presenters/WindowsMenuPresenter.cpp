#include <Presenters/WindowsMenuPresenter.h>
#include <Views/MainToolbarView.h>
#include <Views/RayTracingControlView.h>
#include <Views/NormalsResultView.h>
#include <WidgetRepo.h>

namespace fu {
namespace fusion {

struct WindowsMenuPresenter::Impl
{
	view_ptr_t			m_View;
	rt_ctrl_view_ptr	m_RayControlView;
	norm_res_view_ptr_t m_NormalsResView;
	wrepo_ptr_t			m_Wrepo;

	Impl(
		view_ptr_t view, 
		rt_ctrl_view_ptr rt_ctrl_view, 
		norm_res_view_ptr_t norm_res_view,
		wrepo_ptr_t wrepo)
		: m_View(view)
		, m_RayControlView(rt_ctrl_view)
		, m_NormalsResView(norm_res_view)
		, m_Wrepo(wrepo)
	{ }
};
/// Construction
WindowsMenuPresenter::WindowsMenuPresenter(
	view_ptr_t view, 
	rt_ctrl_view_ptr rt_ctrl_view, 
	norm_res_view_ptr_t norm_res_view,
	wrepo_ptr_t wrepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, rt_ctrl_view, norm_res_view, wrepo))
{ }

void WindowsMenuPresenter::Init()
{
	///==============================
	/// Ray Control Window activated
	///==============================
	m_Impl->m_RayControlView->OnActivated()
		.subscribe([this](auto _) 
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_RayControlView);
	});
	///==============================
	/// Ray Control Window deactivated
	///==============================
	m_Impl->m_RayControlView->OnDeactivated()
		.subscribe([this](auto _) 
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_RayControlView);
	});
	///================================
	/// Normals result view activation
	///================================
	m_Impl->m_NormalsResView->OnActivated()
		.subscribe([this](auto _) 
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_NormalsResView);
	});
	///================================
	/// Normals result view deactivation
	///================================
	m_Impl->m_NormalsResView->OnDeactivated()
		.subscribe([this](auto _)
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_NormalsResView);
	});
	///=================================
	/// Windows menu ray control clicked
	///==================================
	m_Impl->m_View->OnWindowsMenu_RayTracingControlClicked()
		.subscribe([this](auto _) 
	{
		if (m_Impl->m_RayControlView->IsActive())
		{
			m_Impl->m_RayControlView->Deactivate();
		}
		else
		{
			m_Impl->m_RayControlView->Activate();
		}
	});
	///=====================================
	/// windows menu normals result clicked
	///=====================================
	m_Impl->m_View->OnWindowsMenu_NormalsResultClicked()
		.subscribe([this](auto _) 
	{
		if (m_Impl->m_NormalsResView->IsActive())
		{
			m_Impl->m_NormalsResView->Deactivate();
		}
		else
		{
			m_Impl->m_NormalsResView->Activate();
		}
	});
}
}	///	!namespace fusion
}	///	!namespace fu