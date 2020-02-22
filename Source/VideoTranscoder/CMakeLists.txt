cmake_minimum_required(VERSION 3.12)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# cmake modules path
set(ModulesPathRelative "../../cmake")
# get absolute path to modules directory
get_filename_component(ModulesPath "${ModulesPathRelative}" ABSOLUTE)
message(STATUS "Current cmake modules path: ${ModulesPath}")
# set cmake module path to SBF's cmake modules directory
set(CMAKE_MODULE_PATH "${ModulesPath}")
# 3rdParty Directory
set(3rdPartyPathRelative "../../3rdParty")
#get absolute path
get_filename_component(3rdPartyPath "${3rdPartyPathRelative}" ABSOLUTE)
message(STATUS "Current 3rd party path: ${3rdPartyPath}")
# Compiler module
include(GetSystemCompilerId)
# GroupSource module
include(GroupSource)
# PrintList
include(PrintList)
# SBFCommon Project declaration
message(STATUS "Creating cmake project...")
project(
    VideTranscoderProject
    VERSION 1.0.0
    DESCRIPTION "Video Transcoder on top of ffmpeg"
    LANGUAGES CXX
)
#################
# File Gathering
#################
####################
# Free header files
####################
file(GLOB VideoTransFreeHdrFiles "Public/*.h")
GroupSource("${VideoTransFreeHdrFiles}" "Public")
PrintList("${VideoTransFreeHdrFiles}" "Gathered VideoTranscoder free header files")
####################
# Free source files
####################
file(GLOB VideoTransFreeSrcFiles "Private/*.cpp")
GroupSource("${VideoTransFreeSrcFiles}" "Private")
PrintList("${VideoTransFreeSrcFiles}" "Gathered VideoTranscoder free source files")

#####
add_library(
	VideoTranscoder
	STATIC
	"${VideoTransFreeHdrFiles}"
	"${VideoTransFreeSrcFiles}"
)
# Properties
set_target_properties(VideoTranscoder PROPERTIES FOLDER "Fusion360")
target_include_directories(VideoTranscoder PUBLIC "Public")
###############
# Dependencies
###############
#########
# FFMPEG
#########
target_include_directories(VideoTranscoder PUBLIC "${3rdPartyPath}/ffmpeg-cpp/include")
target_include_directories(VideoTranscoder PUBLIC "${3rdPartyPath}/ffmpeg-cpp/ffmpeg/include")
# link to ffmpeg
target_link_libraries(VideoTranscoder ${3rdPartyPath}/ffmpeg-cpp/lib/x64/Debug/ffmpeg-cpp.lib)
target_link_libraries(VideoTranscoder ${3rdPartyPath}/ffmpeg-cpp/lib/x64/Release/ffmpeg-cpp.lib)
#########
# plog
#######
target_include_directories(VideoTranscoder PUBLIC "${3rdPartyPath}/plog/include")
#########
# Common
#########
add_subdirectory("../Common" FusionCommon)
target_link_libraries(VideoTranscoder FusionCommon)
########
# RxCpp
########
# only the include directory suffices
target_include_directories(VideoTranscoder PUBLIC "${3rdPartyPath}/rxcpp/Rx/v2/src")

### Copy binary files to output directory
