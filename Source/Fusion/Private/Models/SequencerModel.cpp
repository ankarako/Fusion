#include <Models/SequencerModel.h>
#include <Models/PlayerModel.h>

#include <limits>;

namespace fu {
namespace fusion {
///	\struct Impl
struct SequencerModel::Impl
{
	size_t m_Duration			= std::numeric_limits<size_t>::max();
	size_t m_VideoTrackPos		= 0;
	size_t m_AnimationTrackPos	= 0;
	size_t m_CurrentFramePos	= 0;

	player_model_ptr_t	m_PlayerModel;

	rxcpp::subjects::subject<size_t> m_PlayerTrackPositionFlowInSubj;
	rxcpp::subjects::subject<size_t> m_AnimationTrackPositionFlowInSubj;

	Impl(player_model_ptr_t player_model)
		: m_PlayerModel(player_model)
	{ }
};	///	!Impl
/// Construction
SequencerModel::SequencerModel(player_model_ptr_t player_model)
	: m_Impl(spimpl::make_unique_impl<Impl>(player_model))
{ }

void SequencerModel::Init()
{

}

void SequencerModel::Start()
{

}

void SequencerModel::Pause()
{

}

void SequencerModel::Stop()
{

}

rxcpp::observer<size_t> fu::fusion::SequencerModel::PlayerTrackPositionFlowIn()
{
	return m_Impl->m_PlayerTrackPositionFlowInSubj.get_subscriber().get_observer().as_dynamic();
}

rxcpp::observer<size_t> fu::fusion::SequencerModel::AnimationTrackPositionFlowIn()
{
	return m_Impl->m_AnimationTrackPositionFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu