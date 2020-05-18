#ifndef __IO_PUBLIC_PERFCAPCALIBRATIONIMPORT_H__
#define __IO_PUBLIC_PERFCAPCALIBRATIONIMPORT_H__

#include <VolcapCameraData.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <string>
#include <fstream>

namespace fu {
namespace io {
///	\brief import perfcap's calibration data from the sprcified file
///	\param	filepath	the file to load the data from
///	\param[output]	calibData	the output calibration data
static void ImportPerfcapCalibration(const std::string& filepath, volcap_cam_data_ptr_t& camData)
{
	using namespace rapidjson;

	std::ifstream inFd(filepath);
	IStreamWrapper isWrapper(inFd);
	Document doc;
	doc.ParseStream(isWrapper);

}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAPCALIBRATIONIMPORT_H__