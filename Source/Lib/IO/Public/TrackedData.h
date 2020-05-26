#ifndef __IO_PUBLIC_TRACKEDDATA_H__
#define __IO_PUBLIC_TRACKEDDATA_H__

#include <Buffer.h>
#include <vector>
#include <memory>

namespace fu {
namespace io {
///	\typedef TrackedSequence
///	\brief a sequence of tracked frames
using TrackedSequence = std::vector<BufferCPU<float>>;
///	\typedef tracked_seq_ptr_t
///	\brief a shared tracked sequence
using tracked_seq_ptr_t = std::shared_ptr<TrackedSequence>;
///	\brief create a tracked sequence ptr
static tracked_seq_ptr_t CreateTrackedSequence()
{
	return std::make_shared<TrackedSequence>();
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_TRACKEDDATA_H__