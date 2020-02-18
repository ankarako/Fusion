#ifndef __APPLICATION_PUBLIC_FINTMANAGER_H__
#define __APPLICATION_PUBLIC_FINTMANAGER_H__

#include <Initializable.h>
#include <spimpl.h>

struct ImFont;

namespace app {
///	\class FontManager
///	\brief Controls font loading etc
class FontManager : public Initializable
{
public:
	///	\enum FontType
	///	\brief represents font type
	enum class FontType
	{
		Big,
		Regular,
		IconsMD_Regular,
		IconsFA_Regular,
		Kenney_Regular,
		ION_Regular,
	};	///	!enum FontType
	///	Construction
	///	\brief Default constructor
	FontManager();
	///	
	FontManager(const FontManager& other) = delete;
	///
	void Init() override;
	///	
	ImFont* GetFont(const FontType fontName) const;
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class FontManager
}	///	!namespace app
#endif	///	!__APPLICATION_PUBLIC_FINTMANAGER_H__