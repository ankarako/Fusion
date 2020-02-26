#include <Models/PlayerModel.h>
#include <Transcoder.h>

/// for converting string to wstring
#include <locale>
#include <codecvt>

namespace fu {
namespace fusion {
///	\brief Video player model implementation
struct PlayerModel::Impl
{
	trans::DecodingContext	m_Context;

	/// Construction
	/// \brief default constructor
	///	does nothing
	Impl() { }
};
///	Construction
///	\brief default constructor
///	Initializes implementation struct
PlayerModel::PlayerModel()
	: m_Impl(spimpl::make_unique_impl<Impl>())
{ }
///	\brief initialize the player
///	Initializes the owned transcoding context
void PlayerModel::Init()
{
	m_Impl->m_Context = trans::dec::CreateContext();
}
///	\brief destroy the player
///	Closes the MFSession
void PlayerModel::Destroy()
{
	
}

void PlayerModel::LoadFile(const std::string& filepath)
{
	std::string f = "file:///" + filepath;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring widefilepath = converter.from_bytes(f);
	trans::dec::LoadSource(m_Impl->m_Context, widefilepath);
}
}	///	!namespace fusion
}	///	!namespace fu