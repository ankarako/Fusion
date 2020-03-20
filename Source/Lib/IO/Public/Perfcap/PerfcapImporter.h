#ifndef __IO_PUBLIC_PERFCAP_PERFCAPIMPORTER_H__
#define __IO_PUBLIC_PERFCAP_PERFCAPIMPORTER_H__

#include <Perfcap/TemplateMesh.h>
#include <Perfcap/AnimationFrame.h>
#include <string>
#include <memory>
#include <spimpl.h>

namespace fu {
namespace io {

class PerfcapImporter
{
public:
	/// Construction
	///	\brief contruct with specified filepath
	///	\param	filepath	path to the perfcap file to import
	PerfcapImporter(const std::string& filepath);
	///	Destruction
	~PerfcapImporter();
	/// \brief the next animation frame
	///	\param[out] frame	a given frame to save the loaded one
	/// \return	true if the frame was read successfully, false otherwise
	bool ReadNextAnimationFrame(AnimationFrame& frame);
	/// \brief Get the template mesh data
	///	\return the template mesh data
	const TemplateMesh& GetTemplateMesh() const;
	///	\brief get the total frame count of the animation
	///	\return the number of frames in the animation sequence
	size_t GetFrameCount() const;
	///	\brief close the reader
	void Close();
private:
	struct Impl;
	spimpl::unique_impl_ptr<Impl> m_Impl;
};	///	!class PerfcapImporter
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAP_PERFCAPIMPORTER_H__