#include <FontManager.h>
#include <map>
#include <imgui.h>
#include <FontDef.h>

namespace fu {
namespace app {
///	\struct Impl
///	\brief FontManager implementation
struct FontManager::Impl
{
	std::map<FontManager::FontType, ImFont*> m_FontMap;
};	///	!struct Impl
///	Construction
///	\brief default constructor
FontManager::FontManager()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
///	\brief Initialization
void FontManager::Init()
{
	ImGuiIO& io = ImGui::GetIO();
	/// Default font BIG
	auto font = io.Fonts->AddFontFromFileTTF("Resources/Fonts/calibri.ttf", 16.0f);
	m_Impl->m_FontMap[FontType::Big] = font;
	/// default font regular
	font = io.Fonts->AddFontFromFileTTF("Resources/Fonts/calibri.ttf", 14.0f);
	m_Impl->m_FontMap[FontType::Regular] = font;
	/// MD icons
	static const ImWchar iconsRange[] = { ICON_MIN_MD, ICON_MAX_MD };
	ImFontConfig iconsConfig;
	iconsConfig.MergeMode = true;
	iconsConfig.PixelSnapH = true;
	iconsConfig.GlyphOffset.y = 5;	/// for vertical alignment with text
	font = io.Fonts->AddFontFromFileTTF("Resources/Fonts/" FONT_ICON_FILE_NAME_MD, 14.0f, &iconsConfig, iconsRange);
	m_Impl->m_FontMap[FontType::IconsMD_Regular] = font;
	font = io.Fonts->AddFontFromFileTTF("Resources/Fonts/" FONT_ICON_FILE_NAME_FAR, 14.0f, &iconsConfig, iconsRange);
}
/// \brief get the specified font type
ImFont* FontManager::GetFont(const FontType name) const
{
	return m_Impl->m_FontMap.at(name);
}
}	///	!namespace app
}	///	!namespace fu