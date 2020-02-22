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
# SBFFusionCommon Project declaration
message(STATUS "Creating cmake project...")
project(
    FusionCommonProject
    VERSION 1.0.0
    DESCRIPTION "FusionCommon Code"
    LANGUAGES CXX
)
#################
# File Gathering
#################
####################
# Free header files
####################
file(GLOB FusionCommonFreeHdrFiles "Public/*.h")
GroupSource("${FusionCommonFreeHdrFiles}" "Public")
PrintList("${FusionCommonFreeHdrFiles}" "Gathered FusionCommon free header files")
####################
# Free source files
####################
file(GLOB FusionCommonFreeSrcFiles "Private/*.cpp")
GroupSource("${FusionCommonFreeSrcFiles}" "Private")
PrintList("${FusionCommonFreeSrcFiles}" "Gathered FusionCommon free source files")

#####
add_library(
	FusionCommon
	STATIC
	"${FusionCommonFreeHdrFiles}"
	"${FusionCommonFreeSrcFiles}"
)
# set to folder
set_target_properties(FusionCommon PROPERTIES FOLDER "Fusion360")
# includes
target_include_directories(FusionCommon PUBLIC "Public")
# dependencies 
############
# OPTIX
########
target_include_directories(FusionCommon PUBLIC "${3rdPartyPath}/NVIDIA/OptiX6.5/include")
target_link_libraries(FusionCommon ${3rdPartyPath}/NVIDIA/OptiX6.5/lib64/optix.6.5.0.lib)
target_link_libraries(FusionCommon ${3rdPartyPath}/NVIDIA/OptiX6.5/lib64/optixu.6.5.0.lib)
# and CUDA
find_package(CUDA)
target_link_libraries(FusionCommon "${CUDA_LIBRARIES}")
target_include_directories(FusionCommon PUBLIC "${CUDA_INCLUDE_DIRS}")
#######
# entt
#######
# header only
target_include_directories(FusionCommon PUBLIC "${3rdPartyPath}/entt/src")
# find_package(Qt5 COMPONENTS Core REQUIRED)
# target_link_libraries(FusionCommon Qt5::Core)
## depends on open frameworks
# add_subdirectory("${3rdPartyPath}/openFrameworks/addons/ofxCMake/project" oframeworks)
# target_link_libraries(FusionCommon oframeworks)