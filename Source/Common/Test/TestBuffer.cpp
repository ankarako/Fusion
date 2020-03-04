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
};	///	!TEST_CASE("Common", "Buffer")