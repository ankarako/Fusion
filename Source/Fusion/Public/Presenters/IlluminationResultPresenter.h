#ifndef	__FUSION_PUBLIC_PRESENTERS_ILLUMINATIONRESULTPRESENTER_H__
#define __FUSION_PUBLIC_PRESENTERS_ILLUMINATIONRESULTPRESENTER_H__

#include <Initializable.h>
#include <spimpl.h>

namespace fu {
namespace fusion {

class IlluminationResultView;
class IlluminationEstimationModel;
class Coordination;

class IlluminationResultPresenter : public app::Initializable
{
public:
	using view_ptr_t = std::shared_ptr<IlluminationResultView>;
	using model_ptr_t = std::shared_ptr<IlluminationEstimationModel>;
	using coord_ptr_t = std::shared_ptr<Coordination>;
	IlluminationResultPresenter(view_ptr_t view, model_ptr_t model, coord_ptr_t coord);
	void Init() override;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_PRESENTERS_ILLUMINATIONRESULTPRESENTER_H__