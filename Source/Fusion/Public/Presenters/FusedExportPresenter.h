#ifndef __FUSION_PUBLIC_PRESENTERS_FUSEDEXPORT_PRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_FUSEDEXPORT_PRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace fusion {

class FusedExportModel;
class PlayerModel;
class PerfcapPlayerModel;
class VideoTracingModel;
class FileExplorerView;
class Coordination;
class OmniconnectModel;

class FusedExportPresenter : public app::Initializable
{
public:
	using model_ptr_t 					= std::shared_ptr<FusedExportModel>;
	using player_model_ptr_t 			= std::shared_ptr<PlayerModel>;
	using perfcap_player_model_ptr_t 	= std::shared_ptr<PerfcapPlayerModel>;
	using video_tracing_model_ptr_t		= std::shared_ptr<VideoTracingModel>;
	using fexp_view_ptr_t				= std::shared_ptr<FileExplorerView>;
	using coord_ptr_t					= std::shared_ptr<Coordination>;
	using omni_model_ptr_t				= std::shared_ptr<OmniconnectModel>;

	FusedExportPresenter(model_ptr_t model,
		player_model_ptr_t player_model,
		perfcap_player_model_ptr_t perfcap_player_model,
		video_tracing_model_ptr_t video_tracing_model,
		fexp_view_ptr_t fexp_view,
		coord_ptr_t coord,
		omni_model_ptr_t omni_model);
	void Init();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FusedExportPresenter
}	///	!namespace fusion 
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_FUSEDEXPORT_PRESENTER_H__