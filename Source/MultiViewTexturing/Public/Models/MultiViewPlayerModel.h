#ifndef __MVT_PUBLIC_MODELS_MULTIVIEWPLAYERMODEL_H__
#define __MVT_PUBLIC_MODELS_MULTIVIEWPLAYERMODEL_H__

#include <Initializable.h>
#include <Destroyable.h>
#include <spimpl.h>
#include <vector>
#include <string>
#include <rxcpp/rx.hpp>
#include <Buffer.h>

namespace fu {
namespace mvt {
///	\class MultiViewPlayerModel
///	\brief handles multi view playback
class MultiViewPlayerModel : public app::Initializable, public app::Destroyable
{
public:
	/// Construction
	///	\brief default constructor (no dependencies)
	MultiViewPlayerModel();
	///	\brief model initialization
	void Init() override;
	void Destroy() override;
	/// io
	rxcpp::observer<std::vector<std::string>>			VideoFilepathsFlowIn();
	rxcpp::observer<int>								SeekFrameIdFlowIn();
	rxcpp::observable<std::vector<BufferCPU<uchar4>>>	MultiViewFramesFlowOut();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class MultiViewPlayerModel
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_MODELS_MULTIVIEWPLAYERMODEL_H__