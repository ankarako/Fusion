#ifndef	__FUSION_PUBLIC_MODELS_WINDOWSMENUMODEL_H__
#define __FUSION_PUBLIC_MODELS_WINDOWSMENUMODEL_H__

#include <Initializable.h>
#include <spimpl.h>
#include <rxcpp/rx.hpp>

namespace fu {
namespace fusion {

class SettingsRepo;

class WindowsMenuModel : public app::Initializable
{
public:
	using srepo_ptr_t = std::shared_ptr<SettingsRepo>;

	WindowsMenuModel(srepo_ptr_t srepo);
	void Init() override;
	rxcpp::observer<bool> NormalsWindowClickedFlowIn();
	rxcpp::observer<bool> IlluminationWindowClickedFlowIn();
	rxcpp::observer<bool> ViewportOptionsWindowClickedFlowIn();
	rxcpp::observable<bool> NormalsWindowVisibleFlowOut();
	rxcpp::observable<bool> IlluminationWindowVisibleFlowOut();
	rxcpp::observable<bool> ViewportOptionsWindowVisibleFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class WindowsMenuModel
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_MODELS_WINDOWSMENUMODEL_H__