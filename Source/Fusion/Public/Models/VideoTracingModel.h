#ifndef	__FUSION_PUBLIC_MODELS_VIDEOTRACINGMODEL_H__
#define __FUSION_PUBLIC_MODELS_VIDEOTRACINGMODEL_H__

#include <Buffer.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {
///	\class VideoTracingModel
///	\brief 3D ray tracing context for rendering 360 videos (or just videos)
class VideoTracingModel
{
public:
	/// \brief rendered buffer output
	// rxcpp::observable<Buffer<>>
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class VideoTracingModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_VIDEOTRACINGMODEL_H__