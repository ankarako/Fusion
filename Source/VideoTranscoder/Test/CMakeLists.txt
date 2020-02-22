cmake_minimum_required(VERSION 3.12)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# cmake modules path
set(ModulesPathRelative "../../../cmake")
# get absolute path to modules directory
get_filename_component(ModulesPath "${ModulesPathRelative}" ABSOLUTE)
message(STATUS "Current cmake modules path: ${ModulesPath}")
# set cmake module path to SBF's cmake modules directory
set(CMAKE_MODULE_PATH "${ModulesPath}")
# 3rdParty Directory
set(3rdPartyPathRelative "../../../3rdParty")
#get absolute path
get_filename_component(3rdPartyPath "${3rdPartyPathRelative}" ABSOLUTE)
message(STATUS "Current 3rd party path: ${3rdPartyPath}")
# Compiler module
include(GetSystemCompilerId)
# GroupSource module
include(GroupSource)
# PrintList
include(PrintList)
# SBFFusionCommon Project declaration
message(STATUS "Creating cmake project...")
project(
    VideoTranscoderTests
    VERSION 1.0.0
    DESCRIPTION "VideoTranscoder Test Code"
    LANGUAGES CXX
)

add_executable(
	TestDecoder
	"TestDecoder.cpp"
)
set_target_properties(TestDecoder PROPERTIES FOLDER "Tests")
add_subdirectory("${3rdPartyPath}/Catch2" Catch2)
target_link_libraries(TestDecoder Catch2)

add_subdirectory("../" VideoTranscoder)
target_link_libraries(TestDecoder VideoTranscoder)