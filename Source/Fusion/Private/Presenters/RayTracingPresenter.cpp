#include <Presenters/RayTracingPresenter.h>
#include <Models/RayTracingModel.h>
#include <Views/FileExplorerView.h>
#include <Views/RayTracingView.h>

namespace fu {
namespace fusion {
///	\struct Impl
///	\brief RayTRacingPresenter's implementation
struct RayTracingPresenter::Impl
{
	fexp_view_ptr_t m_FexpView;
	model_ptr_t		m_Model;
	view_ptr_t		m_View;
	wrepo_ptr_t		m_WRepo;
	/// Construction
	Impl(fexp_view_ptr_t fexpView, model_ptr_t model, view_ptr_t view, wrepo_ptr_t wrepo)
		: m_FexpView(fexpView)
		, m_Model(model)
		, m_View(view)
		, m_WRepo(wrepo)
	{ }
};	///	!struct Impl
/// Construction
RayTracingPresenter::RayTracingPresenter(fexp_view_ptr_t fexpView, model_ptr_t model, view_ptr_t view, wrepo_ptr_t wrepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(fexpView, model, view, wrepo))
{ }
///	\brief initialize the presenter
///	Sets view model tasks
void RayTracingPresenter::Init()
{
	///=====================
	///	View activation task
	///=====================
	m_Impl->m_View->OnActivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_WRepo->RegisterWidget(m_Impl->m_View);
	});
	///=======================
	/// View deactivation task
	///=======================
	m_Impl->m_View->OnDeactivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_WRepo->UnregisterWidget(m_Impl->m_View);
	});
	///==================
	/// Load 3D file task
	///==================
	m_Impl->m_FexpView->Open3DFileFlowOut().subscribe(
		[this](const std::string& filepath) 
	{
		m_Impl->m_Model->LoadAsset(filepath);
	});
	///============================
	/// viewport size changed task
	///===========================
	m_Impl->m_View->OnViewportSizeChanged().subscribe(
		[this](float2 size) 
	{
		uint2 s = make_uint2((unsigned int)size.x, (unsigned int)size.y);
		m_Impl->m_Model->LaunchSizeFlowIn().on_next(s);
	});
	///=================================
	///	tracing  context frame flow out
	///=================================
	m_Impl->m_Model->FrameFlowOut()
		.subscribe(m_Impl->m_View->FrameFlowIn());
	/// activate 3D viewport
	m_Impl->m_View->Activate();
}
}	///	!namespace fusion
}	///	!namespace fu