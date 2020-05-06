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
# SBFCommon Project declaration
message(STATUS "Creating cmake project...")
message(STATUS "============================")
message(STATUS "    Fusion360 Common        ")
message(STATUS "============================")
message(STATUS "Author: Perukas")
project(
    Common
    VERSION 1.0.0
    DESCRIPTION "Fusion360 Common Code"
    LANGUAGES CXX
)
######################
# Gather Source Files
######################
####################
# Free Header Files
####################
file(GLOB CommonFreeHdrFiles "Public/*.h")
GroupSource("${CommonFreeHdrFiles}" "Public")
PrintList("${CommonFreeHdrFiles}" "Gathered Common header files")
####################
# Free Source Files
####################
file(GLOB CommonFreeSrcFiles "Private/*.cpp")
GroupSource("${CommonFreeSrcFiles}" "Private")
PrintList("${CommonFreeSrcFiles}" "Gathered Common source files")
#################
# Create library
#################
add_library(
	Common
	STATIC
	"${CommonFreeHdrFiles}"
	"${CommonFreeSrcFiles}"
)
#####
# set folder propoerty
set_target_properties(Common PROPERTIES FOLDER "Fusion360")
###
# set include directory
set(COMMON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Public")
target_include_directories(Common PUBLIC "Public")
###############
# Dependencies
###############
########
# PLOG
########
# include suffices
target_include_directories(Common PRIVATE "${3rdPartyPath}/plog/include")

target_include_directories(Common PUBLIC "${CUDA_INCLUDE_DIRS}")
target_link_libraries(Common "${CUDA_LIBRARIES}")
message(STATUS "Selected CUDA include directory: ${CUDA_INCLUDE_DIRS}")
message(STATUS "Selected CUDA libraries directory: ${CUDA_LIBRARIES}")