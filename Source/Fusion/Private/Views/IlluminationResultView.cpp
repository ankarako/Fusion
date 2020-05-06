#include <Views/IlluminationResultView.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <GL/gl3w.h>
#include <plog/Log.h>

namespace fu {
namespace fusion {

struct IlluminationResultView::Impl
{
	static constexpr unsigned int k_TextBufferSize = 512;

	float3	m_SampledPosition{ 0.0f, 0.0f, 0.0f };
	uchar4	m_SampledColor{ 0, 0, 0, 1 };
	
	char	m_PositionDisplayTextBuffer[k_TextBufferSize];
	char	m_ColorDisplayTextBuffer[k_TextBufferSize];

	float	m_Radius{ 1.0f };
	float	m_RadiusMin{ 0.0001f };
	float	m_RadiusMax{ 10.0f };
	float	m_Azimuth{ 0.0f };
	float	m_Elevation{ 0.0f };
	float3	m_LightPosition{ 0.0f, 0.0f, 0.0f };
	uchar4	m_LightColor{ 0, 0, 0, 255 };
	
	int m_DisplayTextureWidth{ 512 };
	int m_DisplayTextureHeight{ 256 };
	
	GLuint m_TextureHandle{ 0 };
	BufferCPU<uchar4> m_IlluminationMap;
	rxcpp::subjects::subject<BufferCPU<uchar4>> m_IlluminationMapFlowInSubj;
	
	Impl() { }
};	///	!struct Impl
/// Construction
IlluminationResultView::IlluminationResultView()
	: app::Widget("Illumination View"), m_Impl(spimpl::make_unique_impl<Impl>())
{ }

void IlluminationResultView::Init()
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
	m_Impl->m_IlluminationMapFlowInSubj.get_observable().as_dynamic()
		.subscribe([this](const BufferCPU<uchar4>& img_buf)
	{
		m_Impl->m_IlluminationMap = img_buf;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Impl->m_TextureHandle);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Impl->m_DisplayTextureWidth, m_Impl->m_DisplayTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_buf->Data());
		/// unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	});
	/// initialize text display buffers
	std::memset(m_Impl->m_PositionDisplayTextBuffer, 0, m_Impl->k_TextBufferSize);
	std::memset(m_Impl->m_ColorDisplayTextBuffer, 0, m_Impl->k_TextBufferSize);
	sprintf_s(m_Impl->m_PositionDisplayTextBuffer, "Light Position: %f, %f, %f", 0.0f, 0.0f, 0.0f);
	sprintf_s(m_Impl->m_ColorDisplayTextBuffer, "Light Color: %d, %d, %d", 0, 0, 0);
}

void IlluminationResultView::Render()
{
	bool isActive = this->IsActive();
	if (!isActive)
		return;


	auto winFlags = ImGuiWindowFlags_NoScrollbar;
	ImGui::Begin("Illumination Estimation Result", &isActive, winFlags);
	{

		if (m_Impl->m_TextureHandle)
		{
			ImGui::Image((void*)m_Impl->m_TextureHandle, ImVec2(m_Impl->m_DisplayTextureWidth, m_Impl->m_DisplayTextureHeight));
		}
		ImRect textureRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		ImVec2 mousePos = ImGui::GetMousePos();
		if (ImGui::IsMouseClicked(0) && textureRect.Contains(mousePos))
		{
			/// normalize mouse position
			mousePos.x = (mousePos.x - textureRect.Min.x) / (textureRect.Max.x - textureRect.Min.x);
			mousePos.y = (mousePos.y - textureRect.Min.y) / (textureRect.Max.y - textureRect.Min.y);
			float el = mousePos.y;
			float az = mousePos.x;
			m_Impl->m_Elevation = el;
			m_Impl->m_Azimuth = az;
			/// convert to x, y, z coordintates
			const float x = std::cos(el) * std::cos(az) * m_Impl->m_Radius;
			const float z = std::cos(el) * std::sin(az) * m_Impl->m_Radius;
			const float y = std::sin(el) * m_Impl->m_Radius;
			m_Impl->m_LightPosition = make_float3(x, y, z);
			/// print position value in position display text buffer
			sprintf_s(m_Impl->m_PositionDisplayTextBuffer, "Light Position: x:%f, y:%f, z:%f", x, y, z);
			/// get the buffer's color
			/// convert normalized mouse position to texture coordinates
			unsigned int w = mousePos.x * m_Impl->m_DisplayTextureWidth;
			unsigned int h = mousePos.y * m_Impl->m_DisplayTextureHeight;

			uchar4 color = m_Impl->m_IlluminationMap->Data()[w * h];
			m_Impl->m_LightColor = color;
			sprintf_s(m_Impl->m_ColorDisplayTextBuffer, "Light Color: R:%u, G:%u, B:%u, A:%u", color.x, color.y, color.z, color.w);
		}
		ImGui::Separator();
		ImGui::Text(m_Impl->m_ColorDisplayTextBuffer);
		ImGui::Text(m_Impl->m_PositionDisplayTextBuffer);
		/// Modify the light distance from the origin
		if (ImGui::SliderFloat("Light Distance", &m_Impl->m_Radius, m_Impl->m_RadiusMin, m_Impl->m_RadiusMax))
		{
			/// convert to x, y, z coordintates
			const float x = std::cos(m_Impl->m_Elevation) * std::cos(m_Impl->m_Azimuth) * m_Impl->m_Radius;
			const float z = std::cos(m_Impl->m_Elevation) * std::sin(m_Impl->m_Azimuth) * m_Impl->m_Radius;
			const float y = std::sin(m_Impl->m_Elevation) * m_Impl->m_Radius;
			m_Impl->m_LightPosition = make_float3(x, y, z);
			/// print position value in position display text buffer
			sprintf_s(m_Impl->m_PositionDisplayTextBuffer, "Light Position: x:%f, y:%f, z:%f", x, y, z);
		}
	}
	ImGui::End();
}

rxcpp::observer<BufferCPU<uchar4>> fu::fusion::IlluminationResultView::IlluminationMapFlowIn()
{
	return m_Impl->m_IlluminationMapFlowInSubj.get_subscriber().get_observer().as_dynamic();
}
}	///	!namespace fusion
}	///	!namespace fu