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
# StripFilePathFromList
include(StripPathFromList)
# AppenFilenamesInDir
include(AppendFilenamesInDir)
# copy files from list
include(CopyFilesFromLists)
# SBFCommon Project declaration
message(STATUS "============================")
message(STATUS "  FusionIO Library")
message(STATUS "============================")
message(STATUS "Author: Perukas")
project(
    FusionIOProject
    VERSION 1.0.0
    DESCRIPTION "Fusion Transcoder Library"
    LANGUAGES CXX
)
######################
# Gather Header files
######################
file(GLOB FusionIOHdrFiles "Public/*.h")
GroupSource("${FusionIOHdrFiles}" "Public")
PrintList("${FusionIOHdrFiles}" "Gathered FusionIO header files")
######################
# Gather source files
######################
file(GLOB FusionIOSrcFiles "Private/*.cpp")
GroupSource("${FusionIOSrcFiles}" "Private")
PrintList("${FusionIOSrcFiles}" "Gathered FusionIO source files")
######################
# Gather Perfcap Header files
######################
file(GLOB FusionIOPerfcapHdrFiles "Public/Perfcap/*.h")
GroupSource("${FusionIOPerfcapHdrFiles}" "Public\\Perfcap")
PrintList("${FusionIOPerfcapHdrFiles}" "Gathered FusionIO/Perfcap header files")
######################
# Gather Perfcap source files
######################
file(GLOB FusionIOPerfcapSrcFiles "Private/Perfcap/*.cpp")
GroupSource("${FusionIOPerfcapSrcFiles}" "Private\\Perfcap")
PrintList("${FusionIOPerfcapSrcFiles}" "Gathered FusionIO/Perfcap source files")
###################
# FusionIO library
###################
add_library(
	FusionIO
	STATIC
	"${FusionIOHdrFiles}"
	"${FusionIOSrcFiles}"
	"${FusionIOPerfcapHdrFiles}"
	"${FusionIOPerfcapSrcFiles}"
)
target_include_directories(FusionIO PUBLIC "Public")
# set properties
set_target_properties(FusionIO PROPERTIES FOLDER "Fusion360")
########
# ASSIMP
########
target_include_directories(FusionIO PRIVATE "${3rdPartyPath}/assimp/include")
target_link_libraries(FusionIO debug "${3rdPartyPath}/assimp/lib/assimp-vc142-mtd.lib")
target_link_libraries(FusionIO optimized "${3rdPartyPath}/assimp/lib/assimp-vc142-mt.lib")
target_link_libraries(FusionIO debug "${3rdPartyPath}/assimp/lib/IrrXMLd.lib")
target_link_libraries(FusionIO optimized "${3rdPartyPath}/assimp/lib/IrrXML.lib")
target_link_libraries(FusionIO debug "${3rdPartyPath}/assimp/lib/zlibstaticd.lib")
target_link_libraries(FusionIO optimized "${3rdPartyPath}/assimp/lib/zlibstatic.lib")
#######
# PLOG
#######
target_include_directories(FusionIO PRIVATE "${3rdPartyPath}/plog/include")
#########
# Tinyply
#########
add_subdirectory("${3rdPartyPath}/tinyply" tinyply)
target_link_libraries(FusionIO tinyply)
set_target_properties(tinyply PROPERTIES FOLDER "3rdParty")
################
# TinyOBJLoader
################
add_subdirectory("${3rdPartyPath}/tinyobjloader" tinyobjloader)
target_link_libraries(FusionIO tinyobjloader)
set_target_properties(tinyobjloader PROPERTIES FOLDER "3rdParty")
set_target_properties(example PROPERTIES FOLDER "3rdParty")
################
# spimpl
################
# include dir suffices
target_include_directories(FusionIO PRIVATE "${3rdPartyPath}/spimpl")
#############################
# HPS (for perfcap importer)
#############################
# include suffices
target_include_directories(FusionIO PRIVATE "${3rdPartyPath}/hps/src")
##################################
# TURBOJPEG (for perfcap importer)
##################################
target_include_directories(FusionIO PRIVATE "${3rdPartyPath}/libjpegturbo/include")
target_link_libraries(FusionIO "${3rdPartyPath}/libjpegturbo/lib/jpeg-static.lib")
target_link_libraries(FusionIO "${3rdPartyPath}/libjpegturbo/lib/turbojpeg-static.lib")