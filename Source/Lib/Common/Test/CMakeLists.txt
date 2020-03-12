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
message(STATUS "    Test Common Project")
message(STATUS "============================")
message(STATUS "Author: Perukas")
project(
    TestCommon
    VERSION 1.0.0
    DESCRIPTION "Common code Test Projects"
    LANGUAGES CXX
)
########################
# TestBuffer Executable
########################
add_executable(
	TestCommon_Buffer
	"TestBuffer.cpp"
)
### set properties
set_target_properties(TestCommon_Buffer PROPERTIES FOLDER "Test")
###############
# dependencies
###############
# depends only on common and catch2 of cource, it shouldn't depend on anything else
#########
# COMMON
#########
add_subdirectory("../" Common)
target_link_libraries(TestCommon_Buffer Common)
#########
# CATCH2
#########
add_subdirectory("${3rdPartyPath}/Catch2" Catch2)
##########
# Link TestBuffer to Catch2
target_link_libraries(TestCommon_Buffer Catch2)