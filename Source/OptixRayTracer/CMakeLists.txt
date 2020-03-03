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
message(STATUS "  OptixRayTracer Library")
message(STATUS "============================")
message(STATUS "Author: Perukas")
project(
    OptixRayTracerProject
    VERSION 1.0.0
    DESCRIPTION "Fusion Transcoder Library"
    LANGUAGES CXX
)
######################
# Gather Source Files
######################
###################
# Free header files
###################
file(GLOB OptixRayTracerFreeHdrFiles "Public/*.h")
GroupSource("${OptixRayTracerFreeHdrFiles}" "Public")
PrintList("${OptixRayTracerFreeHdrFiles}" "Gathered OptixRayTracer free header files")
###################
# Free source files
###################
file(GLOB OptixRayTracerFreeSrcFiles "Private/*.cpp")
GroupSource("${OptixRayTracerFreeSrcFiles}" "Private")
PrintList("${OptixRayTracerFreeSrcFiles}" "Gathered OptixRayTracer free header files")
##########################
# Components header files
##########################
file(GLOB OptixRayTracerComponentsHdrFiles "Public/Components/*.h")
GroupSource("${OptixRayTracerComponentsHdrFiles}" "Public\\Components")
PrintList("${OptixRayTracerComponentsHdrFiles}" "Gathered OptixRayTracer/Components header files")
##########################
# Components source files
##########################
file(GLOB OptixRayTracerComponentsSrcFiles "Private/Components/*.cpp")
GroupSource("${OptixRayTracerComponentsSrcFiles}" "Private\\Components")
PrintList("${OptixRayTracerComponentsSrcFiles}" "Gathered OptixRayTracer/Components source files")
##########################
# Systems header files
##########################
file(GLOB OptixRayTracerSystemsHdrFiles "Public/Systems/*.h")
GroupSource("${OptixRayTracerSystemsHdrFiles}" "Public\\Systems")
PrintList("${OptixRayTracerSystemsHdrFiles}" "Gathered OptixRayTracer/Systems header files")
##########################
# Systems source files
##########################
file(GLOB OptixRayTracerSystemsSrcFiles "Private/Systems/*.cpp")
GroupSource("${OptixRayTracerSystemsSrcFiles}" "Private\\Systems")
PrintList("${OptixRayTracerSystemsSrcFiles}" "Gathered OptixRayTracer/Systems source files")
####
add_library(
	OptixRayTracer
	STATIC
	"${OptixRayTracerFreeHdrFiles}"
	"${OptixRayTracerFreeSrcFiles}"
	"${OptixRayTracerComponentsHdrFiles}"
	"${OptixRayTracerComponentsSrcFiles}"
	"${OptixRayTracerSystemsHdrFiles}"
	"${OptixRayTracerSystemsSrcFiles}"
)
### set propertioes
set_target_properties(OptixRayTracer PROPERTIES FOLDER "Fusion360")
# sett public include directory
target_include_directories(OptixRayTracer PUBLIC "Public")
###############
# Dependencies
###############
#######
# CUDA
#######
find_package(CUDA REQUIRED)
target_include_directories(OptixRayTracer PUBLIC "${CUDA_INCLUDE_DIRS}")
target_link_libraries(OptixRayTracer "${CUDA_LIBRARIES}")
message(STATUS "include: ${CUDA_INCLUDE_DIRS}")
message(STATUS "link: ${CUDA_LIBRARIES}")
########
# Optix
########
set(OPTIX_INCLUDE_DIR CACHE STRING "Optix (6.5) include directory")
set(OPTIX_LIBRARIES_DIR CACHE STRING "Optix (6.5) libraries directory")
target_include_directories(OptixRayTracer PUBLIC "${OPTIX_INCLUDE_DIR}")
target_link_libraries(OptixRayTracer "${OPTIX_LIBRARIES_DIR}/optix.6.5.0.lib")
target_link_libraries(OptixRayTracer "${OPTIX_LIBRARIES_DIR}/optixu.6.5.0.lib")
message(STATUS "Added Optix dependency")