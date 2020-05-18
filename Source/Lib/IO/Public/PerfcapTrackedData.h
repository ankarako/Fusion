#ifndef __IO_PUBLIC_PERFCAPTRACKEDDATA_H__
#define __IO_PUBLIC_PERFCAPTRACKEDDATA_H__

#include <Buffer.h>
#include <vector>
#include <array>

namespace fu {
namespace io {
///	\typedef TrackedParams
///	\brief a frames tracked parameters
/// Tracked params is a simple buffer of floats
///	The following scheme is used:
///
///	
///		Root Translation	Root rotation	joint rotations
///		----------------------------------
///		|  x  |	 y  |  z | x  |	y  |  z | - - - - -	
///		----------------------------------
typedef BufferCPU<float> TrackedParams;
///	\typedef TrackedSequence
///	\brief a sequence of TrackedParams
typedef std::vector<TrackedParams> TrackedSequence;
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAPTRACKEDDATA_H__