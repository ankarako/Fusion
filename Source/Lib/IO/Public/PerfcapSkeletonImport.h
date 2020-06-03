#ifndef	__IO_PUBLIC_PERFCAPSKELETONIMPORT_H__
#define __IO_PUBLIC_PERFCAPSKELETONIMPORT_H__

#include <PerfcapAnimationData.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <string>
#include <fstream>
#include <plog/Log.h>

namespace fu {
namespace io {
///	\brief import a perfcap skeleton from a specified file
///	\param	filepath	the file to load the data from
///	\return a perfcap skeleton
static perfcap_skeleton_ptr_t ImportPerfcapSkeleton(const std::string& filepath)
{
	using namespace rapidjson;

	std::ifstream inFd(filepath);

	if (!inFd.good())
	{
		LOG_ERROR << "Failed to read file: " << filepath;
	}
	IStreamWrapper isWrapper(inFd);
	Document doc;
	doc.ParseStream(isWrapper);
	/// create output
	perfcap_skeleton_ptr_t skeleton = CreatePerfcapSkeleton();
	auto obj = doc.GetArray();
	size_t count = obj.Size();
	for (auto it = obj.Begin(); it != obj.End(); ++it)
	{
		perfcap_joint_ptr_t joint = CreatePerfcapJoint();
		auto jObj = it->GetObject();
		if (jObj.HasMember("joint_id"))
		{
			joint->Id = jObj["joint_id"].GetInt();
		}
		if (jObj.HasMember("parent"))
		{
			joint->ParentId = jObj["parent"].GetInt();
		}
		if (jObj.HasMember("children"))
		{
			auto  chArray = jObj["children"].GetArray();
			for (auto chit = chArray.Begin(); chit != chArray.End(); ++chit)
			{
				joint->Children.emplace_back(chit->GetInt());
			}
		}
		if (jObj.HasMember("hierarchy"))
		{
			auto hArray = jObj["hierarchy"].GetArray();
			for (auto hit = hArray.Begin(); hit != hArray.End(); ++hit)
			{
				joint->Hierarchy.emplace_back(hit->GetInt());
			}
		}
		if (jObj.HasMember("swing1"))
		{
			joint->Swing1[0] = jObj["swing1"][0].GetDouble();
			joint->Swing1[1] = jObj["swing1"][1].GetDouble();
			joint->Swing1[2] = jObj["swing1"][2].GetDouble();
		}
		if (jObj.HasMember("swing2"))
		{
			joint->Swing2[0] = jObj["swing2"][0].GetDouble();
			joint->Swing2[1] = jObj["swing2"][1].GetDouble();
			joint->Swing2[2] = jObj["swing2"][2].GetDouble();
		}
		if (jObj.HasMember("twist"))
		{
			joint->Twist[0] = jObj["twist"][0].GetDouble();
			joint->Twist[1] = jObj["twist"][1].GetDouble();
			joint->Twist[2] = jObj["twist"][2].GetDouble();
		}
		if (jObj.HasMember("position"))
		{
			joint->Position[0] = jObj["position"][0].GetDouble();
			joint->Position[1] = jObj["position"][1].GetDouble();
			joint->Position[2] = jObj["position"][2].GetDouble();
		}
		skeleton->emplace_back(joint);
	}
	return skeleton;
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAPSKELETONIMPORT_H__