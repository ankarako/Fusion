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
    FusionLibProject
    VERSION 1.0.0
    DESCRIPTION "Fusion360 Library"
    LANGUAGES CXX
)
#######
# CUDA
#######
find_package(CUDA REQUIRED)

#### ties together library component
add_subdirectory("Common")
add_subdirectory("Application")
add_subdirectory("VideoTranscoder")
add_subdirectory("OptixRayTracer")

cmake_policy(SET CMP0079 NEW)
target_link_libraries(FusionTranscoder Common)
target_link_libraries(OptixRayTracer Common)