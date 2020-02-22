#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <Buffer.h>

TEST_CASE("Buffer", "Creation")
{
	SECTION("Construction")
	{
		/// buffer construction
		static constexpr unsigned int kWidth = 1u;
		static constexpr unsigned int kHeight = 2u;
		static constexpr unsigned int kDepth = 3u;
		static const fu::Dims kBufferDims = fu::Dims(kWidth, kHeight, kDepth);

		fu::Buffer<int, fu::BufferStorageDevice::CPU> myBuffer = fu::CreateBuffer<int, fu::BufferStorageDevice::CPU>(kBufferDims);
	};
	SECTION("Access")
	{
		/// buffer construction
		static constexpr unsigned int kWidth = 1u;
		static constexpr unsigned int kHeight = 2u;
		static constexpr unsigned int kDepth = 3u;
		static const fu::Dims kBufferDims = fu::Dims(kWidth, kHeight, kDepth);
		fu::Buffer<int, fu::BufferStorageDevice::CPU> myBuffer = fu::CreateBuffer<int, fu::BufferStorageDevice::CPU>(kBufferDims);
		REQUIRE(myBuffer->Dimensions().Width == kWidth);
		REQUIRE(myBuffer->Dimensions().Height == kHeight);
		REQUIRE(myBuffer->Dimensions().Depth == kDepth);
		REQUIRE(myBuffer->Count() == kWidth * kHeight * kDepth);
		REQUIRE(myBuffer->ByteSize() == myBuffer->Count() * sizeof(int));
		REQUIRE(myBuffer->Data() != nullptr);
	};
	SECTION("Modification")
	{
		/// buffer construction
		static constexpr unsigned int kWidth = 1u;
		static constexpr unsigned int kHeight = 2u;
		static constexpr unsigned int kDepth = 3u;
		
		static const fu::Dims kBufferDims = fu::Dims(kWidth, kHeight, kDepth);
		
		const std::vector<int> kVector = { 2, 2, 2, 2, 2, 2 };

		fu::Buffer<int, fu::BufferStorageDevice::CPU> myBuffer = fu::CreateBuffer<int, fu::BufferStorageDevice::CPU>(kBufferDims);

		std::memcpy(myBuffer->Data(), kVector.data(), myBuffer->ByteSize());

		int count = myBuffer->Count();
		for (int i = 0; i < count; i++)
		{
			REQUIRE(myBuffer->Data()[i] == 2);
		}
	};
};