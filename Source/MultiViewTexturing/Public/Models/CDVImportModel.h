#ifndef __MVT_PUBLIC_MODELS_CDVIMPORTMODEL_H__
#define __MVT_PUBLIC_MODELS_CDVIMPORTMODEL_H__

#include <spimpl.h>

namespace fu {
namespace mvt {
///	\class CDVImportModel
///	\brief import cdv files
class CDVImportModel
{
public:

private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class CDVImportModel
}	///	!namespace fu
}	///	!namesapce mvt
#endif	///!__MVT_PUBLIC_MODELS_CDVIMPORTMODEL_H__