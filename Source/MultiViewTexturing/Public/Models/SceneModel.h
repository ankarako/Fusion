#ifndef	__MVT_PUBLIC_MODELS_SCENEMODEL_H__
#define __MVT_PUBLIC_MODELS_SCENEMODEL_H__

#include <Initializable.h>
#include <Destroyable.h>
#include <spimpl.h>

namespace fu {
namespace mvt {
///	\class SceneModel
///	\brief holds scene data
class SceneModel : public app::Initializable, public app::Destroyable
{
public:
	///	Construction
	///	\brief default constructor (no dependencies)
	SceneModel();
	/// \brief model initialization
	void Init() override;
	///	\brief model destruction
	void Destroy() override;
	/// IO
	///	inputs
	// rxcpp::observer<>
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class SceneModel
}	///	!namespace mvt
}	///	!namespace fu
#endif	///	!__MVT_PUBLIC_MODELS_SCENEMODEL_H__