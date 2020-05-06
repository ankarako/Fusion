#include <Presenters/WindowsMenuPresenter.h>
#include <Views/MainToolbarView.h>
#include <Views/RayTracingControlView.h>
#include <Views/NormalsResultView.h>
#include <Views/IlluminationResultView.h>
#include <Models/WindowsMenuModel.h>
#include <WidgetRepo.h>

namespace fu {
namespace fusion {

struct WindowsMenuPresenter::Impl
{
	view_ptr_t			m_View;
	rt_ctrl_view_ptr	m_RayControlView;
	norm_res_view_ptr_t m_NormalsResView;
	illum_view_ptr_t	m_IlluminationResView;
	model_ptr_t			m_Model;
	wrepo_ptr_t			m_Wrepo;

	Impl(
		view_ptr_t view,
		rt_ctrl_view_ptr rt_ctrl_view,
		norm_res_view_ptr_t norm_res_view,
		illum_view_ptr_t illum_view,
		model_ptr_t model,
		wrepo_ptr_t wrepo)
		: m_View(view)
		, m_RayControlView(rt_ctrl_view)
		, m_NormalsResView(norm_res_view)
		, m_IlluminationResView(illum_view)
		, m_Model(model)
		, m_Wrepo(wrepo)
	{ }
};
/// Construction
WindowsMenuPresenter::WindowsMenuPresenter(
	view_ptr_t view, 
	rt_ctrl_view_ptr rt_ctrl_view, 
	norm_res_view_ptr_t norm_res_view,
	illum_view_ptr_t illum_view,
	model_ptr_t model,
	wrepo_ptr_t wrepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, rt_ctrl_view, norm_res_view, illum_view, model, wrepo))
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
	///=====================================
	///	Illumination result view activation
	///=====================================
	m_Impl->m_IlluminationResView->OnActivated()
		.subscribe([this](auto _) 
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_IlluminationResView);
	});
	///======================================
	///	Illumination result view deactivation
	///======================================
	m_Impl->m_IlluminationResView->OnDeactivated()
		.subscribe([this](auto _) 
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_IlluminationResView);
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
			m_Impl->m_Model->ViewportOptionsWindowClickedFlowIn().on_next(false);
		}
		else
		{
			m_Impl->m_RayControlView->Activate();
			m_Impl->m_Model->ViewportOptionsWindowClickedFlowIn().on_next(true);
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
			m_Impl->m_Model->NormalsWindowClickedFlowIn().on_next(false);
		}
		else
		{
			m_Impl->m_NormalsResView->Activate();
			m_Impl->m_Model->NormalsWindowClickedFlowIn().on_next(true);
		}
	});
	///=====================================
	/// windows menu normals result clicked
	///=====================================
	m_Impl->m_View->OnWindowsMenu_IlluminationResultClicked()
		.subscribe([this](auto _) 
	{
		if (m_Impl->m_IlluminationResView->IsActive())
		{
			m_Impl->m_IlluminationResView->Deactivate();
			m_Impl->m_Model->IlluminationWindowClickedFlowIn().on_next(false);
		}
		else
		{
			m_Impl->m_IlluminationResView->Activate();
			m_Impl->m_Model->IlluminationWindowClickedFlowIn().on_next(true);
		}
	});
	///===========================================
		/// Model->Normals Window Visible Output Task
		///===========================================
	m_Impl->m_Model->NormalsWindowVisibleFlowOut()
		.subscribe([this](bool visible)
	{
		bool isActive = m_Impl->m_NormalsResView->IsActive();
		if (visible && !isActive)
		{
			m_Impl->m_NormalsResView->Activate();
		}
		else if (!visible && isActive)
		{
			m_Impl->m_NormalsResView->Deactivate();
		}
	});
	///================================================
	/// Model->Illumination Window Visible Output Task
	///================================================
	m_Impl->m_Model->IlluminationWindowVisibleFlowOut()
		.subscribe([this](bool visible)
	{
		bool isActive = m_Impl->m_IlluminationResView->IsActive();
		if (visible && !isActive)
		{
			m_Impl->m_IlluminationResView->Activate();
		}
		else if (!visible && isActive)
		{
			m_Impl->m_IlluminationResView->Deactivate();
		}
	});
	///================================================
	/// Model->Illumination Window Visible Output Task
	///================================================
	m_Impl->m_Model->ViewportOptionsWindowVisibleFlowOut()
		.subscribe([this](bool visible)
	{
		bool isActive = m_Impl->m_RayControlView->IsActive();
		if (visible && !isActive)
		{
			m_Impl->m_RayControlView->Activate();
		}
		else if (!visible && isActive)
		{
			m_Impl->m_RayControlView->Deactivate();
		}
	});
}
}	///	!namespace fusion
}	///	!namespace fu