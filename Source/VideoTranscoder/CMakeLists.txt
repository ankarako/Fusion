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
message(STATUS "============================")
message(STATUS "  FusionTranscoder Library")
message(STATUS "============================")
message(STATUS "Author: Perukas")
project(
    FusionTranscoder
    VERSION 1.0.0
    DESCRIPTION "Fusion Transcoder Library"
    LANGUAGES CXX
)

######################
# Gather Source files
######################
file(GLOB FusionTranscoderFreeHdrFiles "Public/*.h")
GroupSource("${FusionTranscoderFreeHdrFiles}" "Public")
PrintList("${FusionTranscoderFreeHdrFiles}" "Gathered Fusion Transcoder Header files")

file(GLOB FusionTranscoderFreeSrcFiles "Private/*.cpp")
GroupSource("${FusionTranscoderFreeSrcFiles}" "Private")
PrintList("${FusionTranscoderFreeSrcFiles}" "Gathered Fusion Transcoder Source files")

add_library(
	FusionTranscoder
	STATIC
	"${FusionTranscoderFreeHdrFiles}"
	"${FusionTranscoderFreeSrcFiles}"
)
target_include_directories(FusionTranscoder PUBLIC "Public")
set_target_properties(FusionTranscoder PROPERTIES FOLDER "Fusion360")
#############
# Dependences
#############
###################################
# Windows Media Foundation Library
###################################
target_link_libraries(FusionTranscoder "MFPlat.lib")
target_link_libraries(FusionTranscoder "MF.lib")
target_link_libraries(FusionTranscoder "MFuuid.lib")
#########
# Common
#########
add_subdirectory("../Common" Common)
target_link_libraries(FusionTranscoder Common)
#######
# PLog
#######
# adding the include diretory only suffices
target_include_directories(FusionTranscoder PRIVATE "${3rdPartyPath}/plog/include")
message(STATUS "Added plog dependency...")
