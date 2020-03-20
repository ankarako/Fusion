#ifndef __FUSION_PUBLIC_CORE_SEQUENCEITEM_H__
#define __FUSION_PUBLIC_CORE_SEQUENCEITEM_H__

#include <map>
#include <string>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {
enum class SequenceItemType : int
{
	Video		= 0,
	Animation	= 1,
	Static3D	= 2,
	//
	Count
};

static const std::map<SequenceItemType, std::string> k_SequenceItemType2Str = 
{
	{ SequenceItemType::Video,		"Video" },
	{ SequenceItemType::Animation,	"Animation" },
	{ SequenceItemType::Static3D,	"Static 3D" }
};


struct SequenceItem
{
	template <typename T>
	using subj = rxcpp::subjects::subject<T>;


	int					Id;
	std::string			Name;
	SequenceItemType	Type;
	int 				FrameStart;
	int 				FrameEnd;
	int					SeqFrameStart;
	int					SeqFrameEnd;
	bool 				Expanded;
	int					Duration;
	subj<void*>			OnStartPlayback;
	subj<void*>			OnPause;
	subj<void*>			OnStop;
	subj<void*>			OnSeekForward;
	subj<void*>			OnSeekBackward;
};
}	///	!namespace fusion
}	///	!namesapce fu
#endif	///	!__FUSION_PUBLIC_CORE_SEQUENCEITEM_H__