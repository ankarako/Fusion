#include <Models/SceneModel.h>

namespace fu {
namespace mvt {
///	\struct Impl
///	\brief SceneModel Implementation
struct SceneModel::Impl
{
	/// mesh data
	/// skin data
	/// skeleton data
	/// texture data
	/// Construction
	Impl() { }
};	///	!struct Impl
/// Construction
SceneModel::SceneModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
/// \brief model initialization
void SceneModel::Init()
{

}
///	\brief model destruction
void SceneModel::Destroy()
{
	
}
}	///	!namespace mvt
}	///	!namespace fu