#include <Perfcap/PerfcapImporter.h>
#include <Perfcap/AnimationFrame.h>
#include <Perfcap/SkeletonFrame.h>
#include <Perfcap/TextureFrame.h>
#include <Perfcap/TemplateMesh.h>
#include <Perfcap/JpegCompressor.h>
#include <string>
#include <fstream>
#include <hps.h>

namespace fu {
namespace io {
struct PerfcapImporter::Impl
{
	std::ifstream			m_FStream;
	hps::StreamInputBuffer	m_SIB;
	TemplateMesh			m_TemplateMesh;
	AnimationFrame			m_AnimationFrame;
	unsigned int			m_FrameCount;
	unsigned int			m_CurrentFrameIndex;
	utils::JPEGCompressor	m_JpegCompressor;

	Impl(const std::string& filepath)
		: m_FStream(filepath, std::ifstream::binary)
		, m_SIB(m_FStream)
		, m_FrameCount(0)
		, m_CurrentFrameIndex(0)
	{ }
};	///	!struct PerfcapImporter Impl
/// Construction
///	\brief contruct with specified filepath
///	\param	filepath	path to the perfcap file to import
PerfcapImporter::PerfcapImporter(const std::string& filepath)
	: m_Impl(spimpl::make_unique_impl<Impl>(filepath))
{
	m_Impl->m_SIB >> m_Impl->m_TemplateMesh;
	m_Impl->m_SIB >> m_Impl->m_FrameCount;
}
///	Destruction
PerfcapImporter::~PerfcapImporter()
{
	Close();
}
/// \brief the next animation frame
///	\param[out] frame	a given frame to save the loaded one
/// \return	true if the frame was read successfully, false otherwise
bool PerfcapImporter::ReadNextAnimationFrame(AnimationFrame& frame)
{
	if (m_Impl->m_CurrentFrameIndex < m_Impl->m_FrameCount)
	{
		m_Impl->m_SIB >> m_Impl->m_AnimationFrame;

		frame.FrameIndex = m_Impl->m_AnimationFrame.FrameIndex;
		frame.Skeleton = m_Impl->m_AnimationFrame.Skeleton;
		frame.Texture.TextureAttributes = m_Impl->m_AnimationFrame.Texture.TextureAttributes;

		frame.Texture.TextureBuffers.resize(m_Impl->m_AnimationFrame.Texture.TextureBuffers.size());
		for (unsigned int i = 0; i < frame.Texture.TextureBuffers.size(); i++)
		{
			auto image = m_Impl->m_JpegCompressor.decompressImage(
				m_Impl->m_AnimationFrame.Texture.TextureBuffers[i].data(),
				m_Impl->m_AnimationFrame.Texture.TextureBuffers[i].size());
			frame.Texture.TextureBuffers[i].resize(image.height * image.width * 4);
			std::copy(image.data.getBuffer(), image.data.getBuffer() + 4 * image.height * image.width, frame.Texture.TextureBuffers[i].data());
		}
		m_Impl->m_CurrentFrameIndex++;
		return true;
	}
	else
	{
		Close();
		return false;
	}
}
/// \brief Get the template mesh data
///	\return the template mesh data
const TemplateMesh& PerfcapImporter::GetTemplateMesh() const
{
	return m_Impl->m_TemplateMesh;
}
///	\brief get the total frame count of the animation
///	\return the number of frames in the animation sequence
size_t PerfcapImporter::GetFrameCount() const
{
	return m_Impl->m_FrameCount;
}
///	\brief close the reader
void PerfcapImporter::Close()
{
	if (m_Impl->m_FStream.is_open())
		m_Impl->m_FStream.close();
}
size_t fu::io::PerfcapImporter::GetCurrentFrameId() const
{
	return m_Impl->m_CurrentFrameIndex;
}
//const template_mesh_ptr_t PerfcapImporter::GetTemplateMeshPtr() const
//{
//	return std::make_shared<TemplateMesh>(&m_Impl->m_TemplateMesh);
//}
}	///	!namespace io
}	///	!namespace fu