#ifndef	__MVT_PUBLIC_MODELS_VIDEOPLAYERMODEL_H__
#define	__MVT_PUBLIC_MODELS_VIDEOPLAYERMODEL_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace mvt {
///	\class VideoPlayerModel
///	\brief imports videos
class VideoPlayerModel
{
public:
	/// Construction
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class VideoPlayerModel
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_MODELS_VIDEOPLAYERMODEL_H__