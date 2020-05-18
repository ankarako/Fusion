#ifndef __IO_PUBLIC_PERFCAPCALIBRATIONIMPORT_H__
#define __IO_PUBLIC_PERFCAPCALIBRATIONIMPORT_H__

#include <VolcapCameraData.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <string>
#include <fstream>
#include <vector>

namespace fu {
namespace io {
///	\brief import perfcap's calibration data from the sprcified file
///	\param	filepath	the file to load the data from
///	\return	a vector with each device's calibration params
static std::vector<volcap_cam_data_ptr_t> ImportPerfcapCalibration(const std::string& filepath)
{
	using namespace rapidjson;

	std::ifstream inFd(filepath);
	IStreamWrapper isWrapper(inFd);
	Document doc;
	doc.ParseStream(isWrapper);
	/// each device calibrations params are in an array
	auto arr = doc.GetArray();
	/// iterate through the array
	std::vector<volcap_cam_data_ptr_t> importedParams;
	for (auto it = arr.begin(); it != arr.end(); it++)
	{
		volcap_cam_data_ptr_t params = CreateVolcapCameraData();
		auto devObj = it->GetObject();
		/// get device name
		if (devObj.HasMember("name"))
		{
			params->DeviceName = devObj["name"].GetString();
		}
		/// get depth params
		if (devObj.HasMember("depth"))
		{
			auto depthObj = devObj["depth"].GetObject();
			/// extrinsics lie indide an array
			if (depthObj.HasMember("extrinsics"))
			{
				auto extArray = depthObj["extrinsics"].GetArray();
				size_t count = extArray.Size();
				params->DepthExtrinsics = CreateBufferCPU<float>(count);
				for (int i = 0; i < count; ++i)
				{
					params->DepthExtrinsics->Data()[i] = (extArray.Begin() + i)->GetDouble();
				}
			}
			/// intrinsics lie inside an array
			if (depthObj.HasMember("intrinsics"))
			{
				auto intArray = depthObj["intrinsics"].GetArray();
				size_t count = intArray.Size();
				params->DepthIntrinsics = CreateBufferCPU<float>(count);
				for (int i = 0; i < count; ++i)
				{
					params->DepthIntrinsics->Data()[i] = (intArray.Begin() + i)->GetDouble();
				}
			}
		}
		/// get color params
		if (devObj.HasMember("color"))
		{
			auto colorObj = devObj["color"].GetObject();
			/// extrinsics lie inside an array
			if (colorObj.HasMember("extrinsics"))
			{
				auto extArray = colorObj["extrinsics"].GetArray();
				size_t count = extArray.Size();
				params->ColorExtrinsics = CreateBufferCPU<float>(count);
				for (int i = 0; i < count; ++i)
				{
					params->ColorExtrinsics->Data()[i] = (extArray.Begin() + i)->GetDouble();
				}
			}
			if (colorObj.HasMember("intrinsics"))
			{
				auto intArray = colorObj["intrinsics"].GetArray();
				size_t count = intArray.Size();
				params->ColorIntrinsics = CreateBufferCPU<float>(count);
				for (int i = 0; i < count; ++i)
				{
					params->ColorIntrinsics->Data()[i] = (intArray.Begin() + i)->GetDouble();
				}
			}
		}
		importedParams.emplace_back(params);
	}
	return importedParams;
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAPCALIBRATIONIMPORT_H__