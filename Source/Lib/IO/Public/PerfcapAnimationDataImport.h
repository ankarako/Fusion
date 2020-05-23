#ifndef __IO_PUBLIC_PERFCAPANIMATIONDATAIMPORT_H__
#define __IO_PUBLIC_PERFCAPANIMATIONDATAIMPORT_H__

#include <PerfcapAnimationData.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <string>
#include <fstream>

namespace fu {
namespace io {
///	\brief import perfcap's skeleton data (joints and skinning weights)
///	\param	filepath	the file to load the data from
///	\return a perfcap_skin_data object
static perfcap_skin_data_ptr_t ImportPerfcapSkeleton(const std::string& filepath)
{
	using namespace rapidjson;

	std::ifstream inFd(filepath);
	IStreamWrapper isWrapper(inFd);
	Document doc;
	doc.ParseStream(isWrapper);

	perfcap_skin_data_ptr_t skinData = CreatePerfcapSkinData();
	///============
	/// Joint Data
	///============
	auto obj = doc.GetObject();
	if (obj.HasMember("joint_data"))
	{
		auto jointArray = obj["joint_data"].GetArray();
		size_t count = jointArray.Size();
		//skinData->Jointdata = CreateBufferCPU<PerfcapJoint>(count);
	}
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAPANIMATIONDATAIMPORT_H__