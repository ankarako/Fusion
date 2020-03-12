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
########################
# Programs header files
########################
file (GLOB OptixRayTracerProgramsHdrFiles "Public/Programs/*.cuh")
GroupSource("${OptixRayTracerProgramsHdrFiles}" "Public\\Programs")
PrintList("${OptixRayTracerProgramsHdrFiles}" "Gathered Programs header files")
#############################
# Programs CUDA source files
#############################
file (GLOB OptixRayTracerProgramsCuFiles "Private/Programs/*.cu")
GroupSource("${OptixRayTracerProgramsCuFiles}" "Private\\Programs")
PrintList("${OptixRayTracerProgramsCuFiles}" "Gathered Programs cuda source files")

add_library(
	OptixRayTracer
	STATIC
	"${OptixRayTracerFreeHdrFiles}"
	"${OptixRayTracerFreeSrcFiles}"
	"${OptixRayTracerComponentsHdrFiles}"
	"${OptixRayTracerComponentsSrcFiles}"
	"${OptixRayTracerSystemsHdrFiles}"
	"${OptixRayTracerSystemsSrcFiles}"
	"${OptixRayTracerProgramsHdrFiles}"
	"${OptixRayTracerProgramsCuFiles}"
)
target_compile_options(OptixRayTracer PUBLIC "-DNOMINMAX")
target_compile_options(OptixRayTracer PUBLIC "/std:c++latest")
### set propertioes
set_target_properties(OptixRayTracer PROPERTIES FOLDER "Fusion360")
# sett public include directory
target_include_directories(OptixRayTracer PUBLIC "Public")
###############
# Dependencies
###############
########
# Optix
########
set(OPTIX_INCLUDE_DIR CACHE STRING "Optix (6.5) include directory")
set(OPTIX_LIBRARIES_DIR CACHE STRING "Optix (6.5) libraries directory")
# replace backslashes
STRING(REGEX REPLACE "\\\\" "/" OPTIX_INCLUDE_DIR "${OPTIX_INCLUDE_DIR}")
STRING(REGEX REPLACE "\\\\" "/" OPTIX_LIBRARIES_DIR "${OPTIX_LIBRARIES_DIR}")
target_include_directories(OptixRayTracer PUBLIC "${OPTIX_INCLUDE_DIR}")
target_link_libraries(OptixRayTracer "${OPTIX_LIBRARIES_DIR}/optix.6.5.0.lib")
target_link_libraries(OptixRayTracer "${OPTIX_LIBRARIES_DIR}/optixu.6.5.0.lib")
message(STATUS "Selected Optix include directory: ${OPTIX_INCLUDE_DIR}")
message(STATUS "Selected Optix libraries directory: ${OPTIX_LIBRARIES_DIR}")
message(STATUS "Added Optix dependency")
#######
# CUDA
#######
find_package(CUDA REQUIRED)
target_include_directories(OptixRayTracer PUBLIC "${CUDA_INCLUDE_DIRS}")
target_link_libraries(OptixRayTracer "${CUDA_LIBRARIES}")
message(STATUS "Selected CUDA include directory: ${CUDA_INCLUDE_DIRS}")
message(STATUS "Selected CUDA libraries directory: ${CUDA_LIBRARIES}")
####################
# add ptx compiling
####################
# get the first element of the .cuh files list
list(GET OptixRayTracerProgramsHdrFiles 0 FirstCUHFile)
# get the directory (i.e. remove the filename component of the .cuh headers)
get_filename_component(ProgramsIncludeDir "${FirstCUHFile}" DIRECTORY)
set(CUDA_GENERATED_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/../Resources/Programs")
CUDA_INCLUDE_DIRECTORIES("${CUDA_INCLUDE_DIRS}" "${OPTIX_INCLUDE_DIR}" "${ProgramsIncludeDir}")
# we added the cuda targets one-by-one because that's it for now
# TODO: write a more generic script for adding ptx targets
# EnvMapMiss
cuda_compile_ptx(CompilePtxEnvMapMiss "Private/Programs/EnvMapMiss.cu")
add_custom_target(CompilePtxEnvMapMissPTX ALL DEPENDS ${CUDA_GENERATED_OUTPUT_DIR}/EnvMapMiss.ptx "Private/Programs/EnvMapMiss.cu" SOURCES "Private/Programs/EnvMapMiss.cu")
# tedious command to rename the ptx generated files
add_custom_command(TARGET OptixRayTracer POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CompilePtxEnvMapMiss} ${CUDA_GENERATED_OUTPUT_DIR}/EnvMapMiss.ptx )
add_custom_command(TARGET CompilePtxEnvMapMissPTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CompilePtxEnvMapMiss} ${CUDA_GENERATED_OUTPUT_DIR}/EnvMapMiss.ptx )
set_target_properties(CompilePtxEnvMapMissPTX PROPERTIES FOLDER "CUDACustomTargets")
# EnvMapRaygen
cuda_compile_ptx(CompilePtxEnvMapRaygen "Private/Programs/EnvMapRaygen.cu")
add_custom_target(CompilePtxEnvMapRaygenPTX ALL DEPENDS "${CompilePtxEnvMapRaygen}" "Private/Programs/EnvMapRaygen.cu" SOURCES "Private/Programs/EnvMapRaygen.cu")
# tedious command to rename the ptx generated files
add_custom_command(TARGET OptixRayTracer POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CompilePtxEnvMapRaygen} ${CUDA_GENERATED_OUTPUT_DIR}/EnvMapRaygen.ptx )
add_custom_command(TARGET CompilePtxEnvMapRaygenPTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CompilePtxEnvMapRaygen} ${CUDA_GENERATED_OUTPUT_DIR}/EnvMapRaygen.ptx )
set_target_properties(CompilePtxEnvMapRaygenPTX PROPERTIES FOLDER "CUDACustomTargets")
# Pinhole Raygen
cuda_compile_ptx(CompilePtxPinholeRaygen "Private/Programs/PinholeRaygen.cu")
add_custom_target(CompilePtxPinholeRaygenPTX ALL DEPENDS "${CompilePtxPinholeRaygen}" "Private/Programs/PinholeRaygen.cu" SOURCES "Private/Programs/PinholeRaygen.cu")
# tedious command to rename the ptx generated files
add_custom_command(TARGET OptixRayTracer POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CompilePtxPinholeRaygen} ${CUDA_GENERATED_OUTPUT_DIR}/PinholeRaygen.ptx )
add_custom_command(TARGET CompilePtxPinholeRaygenPTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CompilePtxPinholeRaygen} ${CUDA_GENERATED_OUTPUT_DIR}/PinholeRaygen.ptx )
set_target_properties(CompilePtxPinholeRaygenPTX PROPERTIES FOLDER "CUDACustomTargets")
# Triangle Mesh
cuda_compile_ptx(CompilePtxTriangleMesh "Private/Programs/TriangleMesh.cu")
add_custom_target(CompilePtxTriangleMeshPTX ALL DEPENDS "${CompilePtxTriangleMesh}" "Private/Programs/TriangleMesh.cu" SOURCES "Private/Programs/TriangleMesh.cu")
# tedious command to rename the pts generated files
add_custom_command(TARGET OptixRayTracer POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CompilePtxTriangleMesh} ${CUDA_GENERATED_OUTPUT_DIR}/TriangleMesh.ptx )
add_custom_command(TARGET CompilePtxTriangleMeshPTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CompilePtxTriangleMesh} ${CUDA_GENERATED_OUTPUT_DIR}/TriangleMesh.ptx )
set_target_properties(CompilePtxTriangleMeshPTX PROPERTIES FOLDER "CUDACustomTargets")
# Exception
cuda_compile_ptx(CompilePtxException "Private/Programs/Exception.cu")
add_custom_target(CompilePtxExceptionPTX ALL DEPENDS "${CompilePtxException}" "Private/Programs/Exception.cu" SOURCES "Private/Programs/Exception.cu")
add_custom_command(TARGET OptixRayTracer POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CompilePtxException} ${CUDA_GENERATED_OUTPUT_DIR}/Exception.ptx )
add_custom_command(TARGET CompilePtxTriangleMeshPTX POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CompilePtxException} ${CUDA_GENERATED_OUTPUT_DIR}/Exception.ptx )
# tedious command to rename the generated ptx file
set_target_properties(CompilePtxExceptionPTX PROPERTIES FOLDER "CUDACustomTargets")
#######
# PLOG
#######
target_include_directories(OptixRayTracer PRIVATE "${3rdPartyPath}/plog/include")
#########
# Tinyply
#########
add_subdirectory("${3rdPartyPath}/tinyply" tinyply)
target_link_libraries(OptixRayTracer tinyply)
set_target_properties(tinyply PROPERTIES FOLDER "3rdParty")
################
# TinyOBJLoader
################
add_subdirectory("${3rdPartyPath}/tinyobjloader" tinyobjloader)
target_link_libraries(OptixRayTracer tinyobjloader)
set_target_properties(tinyobjloader PROPERTIES FOLDER "3rdParty")
set_target_properties(example PROPERTIES FOLDER "3rdParty")
########
# RxCpp
########
# include dir suffices
target_include_directories(OptixRayTracer PRIVATE "${3rdPartyPath}/rxcpp/Rx/v2/src")
#########
# spimpl
#########
# include dir suffices
target_include_directories(OptixRayTracer PRIVATE "${3rdPartyPath}/spimpl")
#########
# Common
#########
# FIXME: we need Buffer to work like humans
# target_link_libraries(OptixRayTracer PRIVATE Common)
##########################
# Copy files to resources
##########################
# copy ray tracing cuda header files
# this is needed for the runtime ptx compiler
# we use configure since we only need to copy the files
# gather the cuda header files
file(GLOB OptixRayTracerProgramHdrFiles "Public/Programs/*.cuh")
PrintList("${OptixRayTracerProgramHdrFiles}" "Gathered program header files")

StripPathFromList("${OptixRayTracerProgramHdrFiles}" OptixRayTracerProgramHdrFilenames)
AppendFilenamesInDir("${CMAKE_CURRENT_BINARY_DIR}/Resources/Programs" "${OptixRayTracerProgramHdrFilenames}" OptixRayTracerProgramHdrFilesOutputCopyPath)

PrintList("${OptixRayTracerProgramHdrFiles}" "Gathered program header filenames")
PrintList("${OptixRayTracerProgramHdrFilesOutputCopyPath}" "programe header files will be copied to")
# copy the files to Resources directory
# CopyFilesFromLists("${OptixRayTracerProgramHdrFiles}" "${OptixRayTracerProgramHdrFilesOutputCopyPath}")