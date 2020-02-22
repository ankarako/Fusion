#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <DecodingContext.h>

TEST_CASE("Decoding Context", "Decoding")
{
	static fu::trans::DecodingContext myContext;
	static const std::string k_Filepath
		= "E:\\Videos\\He Man\\101 Diamond Ray Of Disappearance.mkv";
	SECTION("Initialization")
	{
		bool init = myContext.Initialize();
		REQUIRE(init == true);
	};
	SECTION("File-opening")
	{
		bool open = myContext.LoadFile(k_Filepath);
		REQUIRE(open == true);
	};
	SECTION("Decode on frame")
	{
		myContext.DecodeCurrentFrame();
	}
	SECTION("Get frame data")
	{
		auto streamData = myContext.GetStreamData(fu::trans::StreamType::Video);
	}
	SECTION("Termination")
	{
		myContext.Terminate();
	}
	SECTION("Re-initialization")
	{
		bool init = myContext.Initialize();
		REQUIRE(init == true);
	}
	SECTION("File-loading after re-initialization")
	{
		bool open = myContext.LoadFile(k_Filepath);
		REQUIRE(open == true);
	}
};