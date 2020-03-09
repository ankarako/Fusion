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
message(STATUS "============================")
message(STATUS " Fusion Application Library")
message(STATUS "============================")
message(STATUS "Author: Perukas")
project(
    FusionApplicationProject
    VERSION 1.0.0
    DESCRIPTION "Fusion360 Application module"
    LANGUAGES CXX
)

message(STATUS "Getting Compiler's Id...")
# Get Compiler id
GetSystemCompiler(CompilerId)
message(STATUS "Current Compiler Id: ${CompilerId}")

###################
# Free header files
###################
file (GLOB FusionApplicationFreeHdrFiles "Public/*.h")
GroupSource("${FusionApplicationFreeHdrFiles}" "Public")
PrintList("${FusionApplicationFreeHdrFiles}" "Gathered Application free header files")
###################
# Free source files
###################
file (GLOB FusionApplicationFreeSrcFiles "Private/*.cpp")
GroupSource("${FusionApplicationFreeSrcFiles}" "Private")
PrintList("${FusionApplicationFreeSrcFiles}" "Gathered Application free header files")

add_library(
	FusionApplication
	STATIC
	"${FusionApplicationFreeHdrFiles}"
	"${FusionApplicationFreeSrcFiles}"
)
# set properties
set_target_properties(FusionApplication PROPERTIES FOLDER "Fusion360")
target_include_directories(FusionApplication PUBLIC "Public")
###############
# Dependencies
###############
#########
# SPIMPL
#########
# depends on spimpl
# we add only the include directory
target_include_directories(FusionApplication PRIVATE "${3rdPartyPath}/spimpl")
#######
# GLFW
#######
# depends on GLFW
add_subdirectory("${3rdPartyPath}/imgui" ImGui)
target_link_libraries(FusionApplication ImGui)
# put glfw in 3rdParty folder
set_target_properties(ImGui PROPERTIES FOLDER "3rdParty")
########
# RxCpp
########
# only the include directory suffices
target_include_directories(FusionApplication PRIVATE "${3rdPartyPath}/rxcpp/Rx/v2/src")
#############
# Font Icons
#############
# only the include dir suffices
target_include_directories(FusionApplication PUBLIC "${3rdPartyPath}/IconFont")
#######
# plog
#######
# include dir suffices
target_include_directories(FusionApplication PRIVATE "${3rdPartyPath}/plog/include")