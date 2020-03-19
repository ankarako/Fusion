#ifndef __FUSION_PUBLIC_CORE_SEQUENCEITEM_H__
#define __FUSION_PUBLIC_CORE_SEQUENCEITEM_H__

#include <map>
#include <string>

namespace fu {
namespace fusion {
enum class SequenceItemType : int
{
	Video		= 0,
	Animation	= 1,
	//
	Count
};

static const std::map<SequenceItemType, const char*> k_SequenceItemType2Str = 
{
	{ SequenceItemType::Video,		"Video" },
	{ SequenceItemType::Animation,	"Animation" }
};


struct SequenceItem
{
	SequenceItemType	Type;
	int 				FrameStart;
	int 				FrameEnd;
	bool 				Expanded;
	int					Duration;
};
}	///	!namespace fusion
}	///	!namesapce fu
#endif	///	!__FUSION_PUBLIC_CORE_SEQUENCEITEM_H__