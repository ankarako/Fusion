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
///	\return	a vector with each device's calibration data
static std::vector<volcap_cam_data_ptr_t> ImportPerfcapCalibration(const std::string& filepath)
{
	using namespace rapidjson;

	std::ifstream inFd(filepath);
	IStreamWrapper isWrapper(inFd);
	Document doc;
	doc.ParseStream(isWrapper);
	/// initialize output
	std::vector<volcap_cam_data_ptr_t> deviceData;
	/// get device array
	auto deviceArray = doc.GetArray();
	size_t deviceCount = deviceArray.Size();
	/// iterate through the device array
	for (int d = 0; d < deviceCount; ++d)
	{
		volcap_cam_data_ptr_t camData = CreateVolcapCameraData();
		if (deviceArray[d].HasMember("name"))
		{
			camData->DeviceName = deviceArray[d]["name"].GetString();
		}
		/// Get depth camera data
		if (deviceArray[d].HasMember("depth"))
		{
			auto dObj = deviceArray[d]["depth"].GetObject();
			/// get extrinsics
			if (dObj.HasMember("extrinsics"))
			{
				auto extr = dObj["extrinsics"].GetArray();
				size_t count = extr.Size();
				camData->DepthExtrinsics = CreateBufferCPU<float>(count);
				for (int i = 0; i < count; ++i)
				{
					camData->DepthExtrinsics->Data()[i] = extr[i].GetDouble();
				}
			}
			/// get intrinsics
			if (dObj.HasMember("intrinsics"))
			{
				auto intr = dObj["intrinsics"].GetArray();
				size_t count = intr.Size();
				camData->DepthIntrinsics = CreateBufferCPU<float>(count);
				for (int i = 0; i < count; ++i)
				{
					camData->DepthIntrinsics->Data()[i] = intr[i].GetDouble();
				}
			}
			if (dObj.HasMember("resolution"))
			{
				if (dObj["resolution"].HasMember("width"))
				{
					camData->DepthResolution.x = dObj["resolution"]["width"].GetInt();
				}
				if (dObj["resolution"].HasMember("height"))
				{
					camData->DepthResolution.y = dObj["resolution"]["height"].GetInt();
				}
			}
		}
		/// Get color camera data
		if (deviceArray[d].HasMember("color"))
		{
			auto dObj = deviceArray[d]["color"].GetObject();
			/// get extrinsics
			if (dObj.HasMember("extrinsics"))
			{
				auto extr = dObj["extrinsics"].GetArray();
				size_t count = extr.Size();
				camData->ColorExtrinsics = CreateBufferCPU<float>(count);
				for (int i = 0; i < count; ++i)
				{
					camData->ColorExtrinsics->Data()[i] = extr[i].GetDouble();
				}
			}
			/// get intrinsics
			if (dObj.HasMember("intrinsics"))
			{
				auto intr = dObj["intrinsics"].GetArray();
				size_t count = intr.Size();
				camData->ColorIntrinsics = CreateBufferCPU<float>(count);
				for (int i = 0; i < count; ++i)
				{
					camData->ColorIntrinsics->Data()[i] = intr[i].GetDouble();
				}
			}
			if (dObj.HasMember("resolution"))
			{
				if (dObj["resolution"].HasMember("width"))
				{
					camData->ColorResolution.x = dObj["resolution"]["width"].GetInt();
				}
				if (dObj["resolution"].HasMember("height"))
				{
					camData->ColorResolution.y = dObj["resolution"]["height"].GetInt();
				}
			}
			if (dObj.HasMember("distortion_coefficients"))
			{
				auto dist = dObj["distortion_coefficients"].GetObject();
				if (dist.HasMember("tangential"))
				{
					auto tang = dist["tangential"].GetObject();
					if (tang.HasMember("p1"))
					{
						camData->TangentialDist.x = tang["p1"].GetDouble();
					}
					if (tang.HasMember("p2"))
					{
						camData->TangentialDist.y = tang["p2"].GetDouble();
					}
				}
				if (dist.HasMember("radial"))
				{
					auto rad = dist["radial"].GetObject();
					if (rad.HasMember("k1"))
					{
						camData->RadialDist123.x = rad["k1"].GetDouble();
					}
					if (rad.HasMember("k2"))
					{
						camData->RadialDist123.y = rad["k2"].GetDouble();
					}
					if (rad.HasMember("k3"))
					{
						camData->RadialDist123.z = rad["k3"].GetDouble();
					}
					if (rad.HasMember("k4"))
					{
						camData->RadialDist345.x = rad["k4"].GetDouble();
					}
					if (rad.HasMember("k5"))
					{
						camData->RadialDist345.y = rad["k5"].GetDouble();
					}
					if (rad.HasMember("k6"))
					{
						camData->RadialDist345.z = rad["k6"].GetDouble();
					}
				}
			}
		}
		deviceData.emplace_back(camData);
	}
	return deviceData;
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAPCALIBRATIONIMPORT_H__