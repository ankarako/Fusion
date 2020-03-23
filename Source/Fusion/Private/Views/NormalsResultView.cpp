#include <Views/NormalsResultView.h>
#include <Core/Coordination.h>
#include <imgui.h>
#include <GL/gl3w.h>
#include <plog/Log.h>

namespace fu {
namespace fusion {

struct NormalsResultView::Impl
{
	coord_ptr_t m_Coordination;
	int m_DisplayTextureWidth{ 512 };
	int m_DisplayTextureHeight{ 256 };

	GLuint m_TextureHandle{ 0 };

	rxcpp::subjects::subject<BufferCPU<uchar4>> m_NormalsImgFlowInSubj;
	/// Construction
	Impl(coord_ptr_t coord)
		: m_Coordination(coord) 
	{ }

};	///	!struct Impl
/// Construiction
NormalsResultView::NormalsResultView(coord_ptr_t coord)
	: app::Widget("Surface Result"), m_Impl(spimpl::make_unique_impl<Impl>(coord))
{ }

void NormalsResultView::Init()
{
	/// Create gl Texture for display
	glGenTextures(1, &m_Impl->m_TextureHandle);
	glBindTexture(GL_TEXTURE_2D, m_Impl->m_TextureHandle);
	/// set magnification and minimization filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	/// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
	///===================================
	/// Result image buffer flow int Task
	///===================================
	m_Impl->m_NormalsImgFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const BufferCPU<uchar4>& img_buf) 
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Impl->m_TextureHandle);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Impl->m_DisplayTextureWidth, m_Impl->m_DisplayTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_buf->Data());
		/// unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	});
}

void NormalsResultView::Render()
{
	bool isActive = this->IsActive();
	if (!isActive)
		return;

	auto winFlags = ImGuiWindowFlags_NoScrollbar;
	ImGui::Begin("Surface Normal Estimation Result", &isActive, winFlags);
	{
		if (m_Impl->m_TextureHandle)
		{
			ImGui::Image((void*)m_Impl->m_TextureHandle, ImVec2(m_Impl->m_DisplayTextureWidth, m_Impl->m_DisplayTextureHeight));
		}
	}
	ImGui::End();
}

rxcpp::observer<BufferCPU<uchar4>> fu::fusion::NormalsResultView::NormalsImageFlowIn()
{
	return m_Impl->m_NormalsImgFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu