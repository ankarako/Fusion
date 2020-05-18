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
    RayMVTProject
    VERSION 1.0.0
    DESCRIPTION "MultiViewTexturing Application"
    LANGUAGES CXX
)
######################
# Gather Source files
######################
####################
# Free Header Files
####################
file (GLOB MVTFreeHdrFiles "Public/*.h")
GroupSource("${MVTFreeHdrFiles}" "Public")
PrintList("${MVTFreeHdrFiles}" "Gathered MVT Free header files")
####################
# Free Source Files
####################
file (GLOB MVTFreeSrcFiles "Private/*.cpp")
GroupSource("${MVTFreeSrcFiles}" "Private")
PrintList("${MVTFreeSrcFiles}" "Gathered MVT Free source files")
##################
# Di Header Files
##################
file (GLOB MVTDIHdrFiles "Public/di/*.h")
GroupSource("${MVTDIHdrFiles}" "Public\\di")
PrintList("${MVTDIHdrFiles}" "Gathered MVT Di header files")
####################
# Core Header Files
####################
file (GLOB MVTCoreHdrFiles "Public/Core/*.h")
GroupSource("${MVTCoreHdrFiles}" "Public\\Core")
PrintList("${MVTCoreHdrFiles}" "Gathered MVT Core header files")
####################
# Core Source Files
####################
file (GLOB MVTCoreSrcFiles "Private/Core/*.cpp")
GroupSource("${MVTCoreSrcFiles}" "Private\\Core")
PrintList("${MVTCoreSrcFiles}" "Gathered MVT Core source files")
######################
# Models Header Files
######################
file (GLOB MVTModelsHdrFiles "Public/Models/*.h")
GroupSource("${MVTModelsHdrFiles}" "Public\\Models")
PrintList("${MVTModelsHdrFiles}" "Gathered MVT Models header files")
######################
# Models Source Files
######################
file (GLOB MVTModelsSrcFiles "Private/Models/*.cpp")
GroupSource("${MVTModelsSrcFiles}" "Private\\Models")
PrintList("${MVTModelsSrcFiles}" "Gathered MVT Models source files")
#########################
# Presenters Header Files
#########################
file (GLOB MVTPresentersHdrFiles "Public/Presenters/*.h")
GroupSource("${MVTPresentersHdrFiles}" "Public\\Presenters")
PrintList("${MVTPresentersHdrFiles}" "Gathered MVT Presenters header files")
#########################
# Presenters Source Files
#########################
file (GLOB MVTPresentersSrcFiles "Private/Presenters/*.cpp")
GroupSource("${MVTPresentersSrcFiles}" "Private\\Presenters")
PrintList("${MVTPresentersSrcFiles}" "Gathered MVT Presenters source files")
#########################
# Views Header Files
#########################
file (GLOB MVTViewsHdrFiles "Public/Views/*.h")
GroupSource("${MVTViewsHdrFiles}" "Public\\Views")
PrintList("${MVTViewsHdrFiles}" "Gathered MVT Views header files")
####################
# Views Source Files
####################
file (GLOB MVTViewsSrcFiles "Private/Views/*.cpp")
GroupSource("${MVTViewsSrcFiles}" "Private\\Views")
PrintList("${MVTViewsSrcFiles}" "Gathered MVT Views source files")
########################
# Settings Header files
########################
file (GLOB MVTSettingsHdrFiles "Public/Settings/*.h")
GroupSource("${MVTSettingsHdrFiles}" "Public\\Settings")
PrintList("${MVTSettingsHdrFiles}" "Gathered MVT Settings header files")
########################
# Settings Src files
########################
file (GLOB MVTSettingsSrcFiles "Private/Settings/*.cpp")
GroupSource("${MVTSettingsSrcFiles}" "Private\\Settings")
PrintList("${MVTSettingsSrcFiles}" "Gathered MVT Settings source files")
#################
# MVT Project
#################
### optix need no min max on windows
add_compile_options("-DNOMINMAX")
add_executable(
	RayMVT
	"Source/main.cpp"
	"${MVTFreeHdrFiles}"
	"${MVTFreeSrcFiles}"
	"${MVTDIHdrFiles}"
	"${MVTCoreHdrFiles}"
	"${MVTCoreSrcFiles}"
	"${MVTModelsHdrFiles}"
	"${MVTModelsSrcFiles}"
	"${MVTPresentersHdrFiles}"
	"${MVTPresentersSrcFiles}"
	"${MVTViewsHdrFiles}"
	"${MVTViewsSrcFiles}"
	"${MVTSettingsHdrFiles}"
	"${MVTSettingsSrcFiles}"
)
set_target_properties(RayMVT PROPERTIES FOLDER "RayMVT")
set_property(TARGET RayMVT PROPERTY CXX_STANDARD 17)
# set include directory
target_include_directories(RayMVT PUBLIC "Public")
###############
# Dependencies
###############
# di
# just the include path suffices
target_include_directories(RayMVT PUBLIC "${3rdPartyPath}/boost-di/include")
# add the library
add_subdirectory("../Lib" FusionLib)
target_link_libraries(RayMVT PRIVATE FusionApplication)
target_link_libraries(RayMVT PRIVATE OptixRayTracer)
target_link_libraries(RayMVT PRIVATE FusionIO)
# ImGuizmo
cmake_policy(SET CMP0079 NEW)
add_subdirectory("${3rdPartyPath}/imguizmo" ImGuizmo)
target_link_libraries(ImGuizmo PRIVATE ImGui)
target_link_libraries(RayMVT PRIVATE ImGuizmo)
# depends on spimpl
# we add only the include directory
target_include_directories(RayMVT PRIVATE "${3rdPartyPath}/spimpl")
########
# RxCpp
########
# only the include directory suffices
target_include_directories(RayMVT PRIVATE "${3rdPartyPath}/rxcpp/Rx/v2/src")
#######
# PLog
#######
# adding the include diretory only suffices
target_include_directories(RayMVT PRIVATE "${3rdPartyPath}/plog/include")
############
# RapidJson
############
target_include_directories(RayMVT PRIVATE "${3rdPartyPath}/rapidjson/include")
######
# STB
#####
target_include_directories(RayMVT PUBLIC "${3rdPartyPath}/stb")
######
# HPS
#####
target_include_directories(RayMVT PRIVATE "${3rdPartyPath}/hps/src")
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
# Copy 7zip files
configure_file(
	"${CMAKE_SOURCE_DIR}/../../Resources/7zip/7z.exe"
	"${CMAKE_CURRENT_BINARY_DIR}/Resources/7zip/7z.exe" COPYONLY)
configure_file(
	"${CMAKE_SOURCE_DIR}/../../Resources/7zip/7z.dll"
	"${CMAKE_CURRENT_BINARY_DIR}/Resources/7zip/7z.dll" COPYONLY)
configure_file(
	"${CMAKE_SOURCE_DIR}/../../Resources/7zip/7-zip.dll"
	"${CMAKE_CURRENT_BINARY_DIR}/Resources/7zip/7-zip.dll" COPYONLY)
configure_file(
	"${CMAKE_SOURCE_DIR}/../../Resources/7zip/7-zip32.dll"
	"${CMAKE_CURRENT_BINARY_DIR}/Resources/7zip/7-zip32.dll" COPYONLY)
