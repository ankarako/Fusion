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
message(STATUS "    Fusion360 Application")
message(STATUS "============================")
message(STATUS "Author: Perukas")
project(
    FusionProject
    VERSION 1.0.0
    DESCRIPTION "Fusion360 Application"
    LANGUAGES CXX
)
###########################
# Gather free header files
###########################
file (GLOB FusionFreeHdrFiles "Public/*.h")
GroupSource("${FusionFreeHdrFiles}" "Public")
PrintList("${FusionFreeHdrFiles}" "Gathered Fusion free header files")
###########################
# Gather free source files
###########################
file (GLOB FusionFreeSrcFiles "Private/*.cpp")
GroupSource("${FusionFreeSrcFiles}" "Private")
PrintList("${FusionFreeSrcFiles}" "Gathered Fusion free source files")
#########################
# Gather DI header files
#########################
file (GLOB FusionDiHdrFiles "Public/di/*.h")
GroupSource("${FusionDiHdrFiles}" "Public\\di")
PrintList("${FusionDiHdrFiles}" "Gathered Fusion di header files")
#############################
# Gather models header files
#############################
file (GLOB FusionModelsHdrFiles "Public/Models/*.h")
GroupSource("${FusionModelsHdrFiles}" "Public\\Models")
PrintList("${FusionModelsHdrFiles}" "Gathered Model header files")
#############################
# Gather models source files
#############################
file (GLOB FusionModelsSrcFiles "Private/Models/*.cpp")
GroupSource("${FusionModelsSrcFiles}" "Private\\Models")
PrintList("${FusionModelsSrcFiles}" "Gathered Model source files")
#############################
# Gather presenters header files
#############################
file (GLOB FusionPresentersHdrFiles "Public/Presenters/*.h")
GroupSource("${FusionPresentersHdrFiles}" "Public\\Presenters")
PrintList("${FusionPresentersHdrFiles}" "Gathered Presenter header files")
#############################
# Gather presenters source files
#############################
file (GLOB FusionPresentersSrcFiles "Private/Presenters/*.cpp")
GroupSource("${FusionPresentersSrcFiles}" "Private\\Presenters")
PrintList("${FusionPresentersSrcFiles}" "Gathered PResenters source files")
#############################
# Gather Views header files
#############################
file (GLOB FusionViewsHdrFiles "Public/Views/*.h")
GroupSource("${FusionViewsHdrFiles}" "Public\\Views")
PrintList("${FusionViewsHdrFiles}" "Gathered View header files")
#############################
# Gather Views source files
#############################
file (GLOB FusionViewsSrcFiles "Private/Views/*.cpp")
GroupSource("${FusionViewsSrcFiles}" "Private\\Views")
PrintList("${FusionViewsSrcFiles}" "Gathered Views source files")
#############################
# Gather Core header files
#############################
file (GLOB FusionCoreHdrFiles "Public/Core/*.h")
GroupSource("${FusionCoreHdrFiles}" "Public\\Core")
PrintList("${FusionCoreHdrFiles}" "Gathered Core header files")
#############################
# Gather Core source files
#############################
file (GLOB FusionCoreSrcFiles "Private/Core/*.cpp")
GroupSource("${FusionCoreSrcFiles}" "Private\\Core")
PrintList("${FusionCoreSrcFiles}" "Gathered Core source files")
#############################
# Gather Core UI header files
#############################
file (GLOB FusionCoreUIHdrFiles "Public/Core/UI/*.h")
GroupSource("${FusionCoreUIHdrFiles}" "Public\\Core\\UI")
PrintList("${FusionCoreUIHdrFiles}" "Gathered Core header files")
#############################
# Gather Core source files
#############################
file (GLOB FusionCoreUISrcFiles "Private/Core/UI/*.cpp")
GroupSource("${FusionCoreUISrcFiles}" "Private\\Core\\UI")
PrintList("${FusionCoreUISrcFiles}" "Gathered Core source files")
#############################
# Gather Settings header files
#############################
file (GLOB FusionSettingsHdrFiles "Public/Settings/*.h")
GroupSource("${FusionSettingsHdrFiles}" "Public\\Settings")
PrintList("${FusionSettingsHdrFiles}" "Gathered Settings header files")
#############################
# Gather Settings source files
#############################
file (GLOB FusionSettingsSrcFiles "Private/Settings/*.cpp")
GroupSource("${FusionSettingsSrcFiles}" "Private\\Settings")
PrintList("${FusionSettingsSrcFiles}" "Gathered Settings source files")
#################
# Fusion Project
#################

add_executable(
	Fusion
	"Source/main.cpp"
	"${FusionFreeHdrFiles}"
	"${FusionFreeSrcFiles}"
	"${FusionDiHdrFiles}"
	"${FusionModelsHdrFiles}"
	"${FusionModelsSrcFiles}"
	"${FusionPresentersHdrFiles}"
	"${FusionPresentersSrcFiles}"
	"${FusionViewsHdrFiles}"
	"${FusionViewsSrcFiles}"
	"${FusionCoreHdrFiles}"
	"${FusionCoreSrcFiles}"
	"${FusionCoreUIHdrFiles}"
	"${FusionCoreUISrcFiles}"
	"${FusionSettingsHdrFiles}"
	"${FusionSettingsSrcFiles}"
)
#### set properties
set_target_properties(Fusion PROPERTIES FOLDER "Fusion360")
set_property(TARGET Fusion PROPERTY CXX_STANDARD 17)
# set include directory
target_include_directories(Fusion PUBLIC "Public")
add_definitions(-DNOMINMAX)
###############
# Dependencies
###############
# di
# adding the include directory suffices
target_include_directories(Fusion PUBLIC "${3rdPartyPath}/boost-di/include")
message(STATUS "Added boost-di dependency...")
# Application module
add_subdirectory("../Application" FusionApplication)
target_link_libraries(Fusion FusionApplication)
message(STATUS "Added FusionApplication dependency...")
# depends on spimpl
# we add only the include directory
target_include_directories(Fusion PRIVATE "${3rdPartyPath}/spimpl")
message(STATUS "Added spimpl dependency...")
########
# RxCpp
########
# only the include directory suffices
target_include_directories(Fusion PRIVATE "${3rdPartyPath}/rxcpp/Rx/v2/src")
message(STATUS "Added rxcpp dependency...")
#######
# PLog
#######
# adding the include diretory only suffices
target_include_directories(Fusion PRIVATE "${3rdPartyPath}/plog/include")
message(STATUS "Added plog dependency...")
############
# RapidJson
############
target_include_directories(Fusion PRIVATE "${3rdPartyPath}/rapidjson/include")
message(STATUS "Added rapidjson dependency...")
# target_link_libraries(Fusion FusionCommon)
######
# STB
#####
target_include_directories(Fusion PUBLIC "${3rdPartyPath}/stb")
message(STATUS "Added stb dependency...")
###################
# FusionTranscoder
###################
add_subdirectory("../VideoTranscoder" FusionTranscoder)
target_link_libraries(Fusion FusionTranscoder)
message(STATUS "Added FusionTranscoder dependency...")
##################
# OptixRayTracer
#################
add_subdirectory("../OptixRayTracer" OptixRayTracer)
target_link_libraries(Fusion OptixRayTracer)
message(STATUS "Added OptixRayTracer dependency")
########
# Optix
########
# set(OPTIX_INCLUDE_DIR CACHE STRING "Optix (6.5) include directory")
# set(OPTIX_LIBRARIES_DIR CACHE STRING "Optix (6.5) libraries directory")
# # replace backslashes
# STRING(REGEX REPLACE "\\\\" "/" OPTIX_INCLUDE_DIR "${OPTIX_INCLUDE_DIR}")
# STRING(REGEX REPLACE "\\\\" "/" OPTIX_LIBRARIES_DIR "${OPTIX_LIBRARIES_DIR}")
# target_include_directories(OptixRayTracer PUBLIC "${OPTIX_INCLUDE_DIR}")
# target_link_libraries(OptixRayTracer "${OPTIX_LIBRARIES_DIR}/optix.6.5.0.lib")
# target_link_libraries(OptixRayTracer "${OPTIX_LIBRARIES_DIR}/optixu.6.5.0.lib")
# message(STATUS "Selected Optix include directory: ${OPTIX_INCLUDE_DIR}")
# message(STATUS "Selected Optix libraries directory: ${OPTIX_LIBRARIES_DIR}")
# message(STATUS "Added Optix dependency")
#######
# CUDA
#######
find_package(CUDA REQUIRED)
target_include_directories(Fusion PUBLIC "${CUDA_INCLUDE_DIRS}")
target_link_libraries(Fusion "${CUDA_LIBRARIES}")
message(STATUS "Selected CUDA include directory: ${CUDA_INCLUDE_DIRS}")
message(STATUS "Selected CUDA libraries directory: ${CUDA_LIBRARIES}")
#### copy resources
# we use configure_file() instead file() because it creates a 
# file-level dependency
# \see: https://stackoverflow.com/questions/34799916/copy-file-from-source-directory-to-binary-directory-using-cmake
configure_file(
	"${CMAKE_SOURCE_DIR}/../../Resources/Fonts/calibri.ttf" 
	"${CMAKE_CURRENT_BINARY_DIR}/Resources/Fonts/calibri.ttf" COPYONLY)
configure_file(
	"${CMAKE_SOURCE_DIR}/../../Resources/Fonts/fa-regular-400.ttf" 
	"${CMAKE_CURRENT_BINARY_DIR}/Resources/Fonts/fa-regular-400.ttf" COPYONLY)
configure_file(
	"${CMAKE_SOURCE_DIR}/../../Resources/Fonts/MaterialIcons-Regular.ttf" 
	"${CMAKE_CURRENT_BINARY_DIR}/Resources/Fonts/MaterialIcons-Regular.ttf" COPYONLY)
###
# create the new directories for the files
# copy optix cuda program files
