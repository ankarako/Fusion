#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <Buffer.h>

TEST_CASE("Common", "Buffer")
{
	SECTION("Buffer", "Device::CPU")
	{
		static constexpr size_t	kBufferElmntCount = 16;
		{
			fu::BufferCPU<int> myBuffer = fu::CreateBufferCPU<int>(kBufferElmntCount);
			REQUIRE(myBuffer->Count() == kBufferElmntCount);
			REQUIRE(myBuffer->Stride() == sizeof(int));
			REQUIRE(myBuffer->ByteSize() == kBufferElmntCount * sizeof(int));
			REQUIRE(myBuffer->Data() != nullptr);

			for (int i = 0; i < kBufferElmntCount; i++)
			{
				myBuffer->Data()[i] = i;
			}
			for (int i = 0; i < kBufferElmntCount; i++)
			{
				REQUIRE(myBuffer->Data()[i] == i);
			}
		}
	};	///	!SECTION("Buffer", "CPU")
	SECTION("Buffer", "Device::GPU")
	{
		static constexpr size_t	kBufferElmntCount = 16;
		fu::BufferGPU<int> myBuffer = fu::CreateBufferGPU<int>(kBufferElmntCount);
		REQUIRE(myBuffer->Count() == kBufferElmntCount);
		REQUIRE(myBuffer->Stride() == sizeof(int));
		REQUIRE(myBuffer->ByteSize() == kBufferElmntCount * sizeof(int));
		REQUIRE(myBuffer->Data() != nullptr);
	};
	SECTION("Buffer: CPU->CPU", "Copy")
	{
		static constexpr size_t	kBufferElmntCount = 16;
		fu::BufferCPU<int> srcBuffer = fu::CreateBufferCPU<int>(kBufferElmntCount);
		fu::BufferCPU<int> dstBuffer = fu::CreateBufferCPU<int>(kBufferElmntCount);
		for (int i = 0; i < kBufferElmntCount; i++)
		{
			srcBuffer->Data()[i] = i;
		}
		fu::BufferCopy(dstBuffer, srcBuffer);
		for (int i = 0; i < kBufferElmntCount; i++)
		{
			REQUIRE(dstBuffer->Data()[i] == i);
		}
	};	///	!SECTION("Buffer: CPU->CPU", "Copy")
	SECTION("Buffer: CPU->GPU", "Copy")
	{
		static constexpr size_t	kBufferElmntCount = 16;
		fu::BufferCPU<int> srcBuffer = fu::CreateBufferCPU<int>(kBufferElmntCount);
		fu::BufferGPU<int> dstBuffer = fu::CreateBufferGPU<int>(kBufferElmntCount);
		for (int i = 0; i < kBufferElmntCount; i++)
		{
			srcBuffer->Data()[i] = i;
		}
		fu::BufferCopy(dstBuffer, srcBuffer);
	};	///	!SECTION("Buffer: CPU->GPU", "Copy")
};	///	!TEST_CASE("Common", "Buffer")