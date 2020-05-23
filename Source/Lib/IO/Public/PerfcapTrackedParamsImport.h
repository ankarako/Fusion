#ifndef __IO_PUBLIC_PERFCAPTRACKEDPARAMSIMPORT_H__
#define __IO_PUBLIC_PERFCAPTRACKEDPARAMSIMPORT_H__

#include <TrackedData.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <string>
#include <fstream>
#include <plog/Log.h>

namespace fu {
namespace io {
static tracked_seq_ptr_t ImportPerfcapTrackedParams(const std::string& filepath)
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

	tracked_seq_ptr_t trackedParams = CreateTrackedSequence();
	auto obj = doc.GetArray();

	for (auto it = obj.Begin(); it != obj.End(); ++it)
	{
		auto frameData = it->GetObject();
		if (frameData.HasMember("raw_params"))
		{
			auto params = frameData["raw_params"].GetArray();
			size_t paramsCount = params.Size();
			BufferCPU<float> frame = CreateBufferCPU<float>(paramsCount);
			for (int p = 0; p < paramsCount; ++p)
			{
				frame->Data()[p] = (float)(params.Begin() + p)->GetDouble();
			}
			trackedParams->emplace_back(frame);
		}
	}
	return trackedParams;
}
}	///	!namespace io
}	///	!namespace fu
#endif	///	!__IO_PUBLIC_PERFCAPTRACKEDPARAMSIMPORT_H__