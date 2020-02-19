#include <Presenters/FileExplorerPresenter.h>
#include <Models/FileExplorerModel.h>
#include <Views/FileExplorerView.h>
#include <WidgetRepo.h>

namespace fusion {
struct FileExplorerPresenter::Impl
{
	model_ptr_t	m_Model;
	view_ptr_t	m_View;
	wrepo_ptr_t m_Wrepo;

	Impl(model_ptr_t model, view_ptr_t view, wrepo_ptr_t wrepo)
		: m_Model(model), m_View(view), m_Wrepo(wrepo)
	{ }
};	

FileExplorerPresenter::FileExplorerPresenter(model_ptr_t model, view_ptr_t view, wrepo_ptr_t wrepo)
	: m_Impl(spimpl::make_unique_impl<Impl>(model, view, wrepo))
{ }

void FileExplorerPresenter::Init()
{
	m_Impl->m_View->OnActivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_View);
	});

	m_Impl->m_View->OnDeactivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_View);
	});

	m_Impl->m_Model->CurrentDirectoryFlowOut().subscribe(m_Impl->m_View->CurrentDirectoryFlowIn());
	m_Impl->m_Model->CurrentDirEntriesFlowOut().subscribe(m_Impl->m_View->CurrentDirEntriesFlowIn());
	m_Impl->m_Model->CurrentDirHierarchyFlowOut().subscribe(m_Impl->m_View->CurrentDirHierarchyIn());
	m_Impl->m_Model->DriveCountFlowOut().subscribe(m_Impl->m_View->DriveCountFlowIn());
	m_Impl->m_Model->DriveLettersFlowOut().subscribe(m_Impl->m_View->DriveLettersFlowIn());
}
}	///	!namespace fusion