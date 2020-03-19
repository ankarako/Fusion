#include <Presenters/SequencerPresenter.h>
#include <Views/SequencerView.h>
#include <Models/PlayerModel.h>
#include <WidgetRepo.h>

namespace fu {
namespace fusion {

struct SequencerPresenter::Impl
{
	view_ptr_t		m_View;
	wrepo_ptr_t		m_Wrepo;
	player_ptr_t	m_PlayerModel;

	Impl(view_ptr_t view, wrepo_ptr_t wrepo, player_ptr_t player)
		: m_View(view), m_Wrepo(wrepo), m_PlayerModel(player)
	{ }
};	///	!struct Impl
/// Construction
SequencerPresenter::SequencerPresenter(view_ptr_t view, wrepo_ptr_t wrepo, player_ptr_t player)
	: m_Impl(spimpl::make_unique_impl<Impl>(view, wrepo, player))
{ }

void SequencerPresenter::Init()
{
	///===================
	/// view activation
	///=================
	m_Impl->m_View->OnActivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_Wrepo->RegisterWidget(m_Impl->m_View);
	});
	///===================
	/// view deactivation
	///===================
	m_Impl->m_View->OnDeactivated().subscribe(
		[this](auto _) 
	{
		m_Impl->m_Wrepo->UnregisterWidget(m_Impl->m_View);
	});
	///==============================
	/// PlayerModel Loaded video Task
	///==============================
	m_Impl->m_PlayerModel->SequenceItemFlowOut()
		.subscribe(m_Impl->m_View->SequencerItemFlowIn());

	m_Impl->m_View->Activate();
}

}	///	@namesapce fusion
}	///	!namesapce fu