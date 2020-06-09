#ifndef __IO_PUBLIC_PERFCAPSKINNINGDATAIMPORT_H__
#define __IO_PUBLIC_PERFCAPSKINNINGDATAIMPORT_H__

#include <PerfcapAnimationData.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <string>
#include <fstream>
#include <vector>
#include <plog/Log.h>

#ifdef GetObject
#undef GetObject
#endif	

namespace fu {
namespace io {
///	\brief import perfcap's skinning data from the specified file
///	\param	filepath	the file to load the data from
///	\return
static perfcap_skin_data_ptr_t ImportPerfcapSkinningData(const std::string& filepath)
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
	perfcap_skin_data_ptr_t skinData = CreatePerfcapSkinData();

	auto obj = doc.GetObject();
	if (obj.HasMember("skinning_data"))
	{
		auto skinObj = obj["skinning_data"].GetObject();
		if (skinObj.HasMember("num_vertices"))
		{
			skinData->NumVertices = skinObj["num_vertices"].GetInt();
		}
		if (skinObj.HasMember("num_weights_per_vertex"))
		{
			skinData->NumWeightsPerVertex = skinObj["num_weights_per_vertex"].GetInt();
		}
		if (skinObj.HasMember("data"))
		{
			auto dataArr = skinObj["data"].GetArray();
			int dataCount = skinData->NumVertices * skinData->NumWeightsPerVertex;
			int vcount = skinData->NumVertices;
			int wcount = skinData->NumWeightsPerVertex;
			skinData->WeightData.reserve(dataCount);
			skinData->Jointdata.reserve(dataCount);
			for (int v = 0; v < vcount; ++v)
			{
				auto vData = dataArr[v].GetObject();
				if (vData.HasMember("weights"))
				{
					auto warray = vData["weights"].GetArray();
					for (int w = 0; w < skinData->NumWeightsPerVertex; ++w)
					{
						skinData->WeightData.emplace_back(warray[w].GetDouble());
					}
				}
				if (vData.HasMember("ids"))
				{
					auto warray = vData["ids"].GetArray();
					for (int w = 0; w < skinData->NumWeightsPerVertex; ++w)
					{
						skinData->Jointdata.emplace_back(warray[w].GetInt());
					}
				}
			}
		}
	}
	return skinData;
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAPSKINNINGDATAIMPORT_H__
