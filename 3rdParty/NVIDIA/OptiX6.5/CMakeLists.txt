cmake_minimum_required(VERSION 3.12)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project (
	OptixProject
    VERSION 1.0.0
    DESCRIPTION "Dummy optix project"
    LANGUAGES CXX
)

add_library(Optix_Shared "bin64/optix.6.5.0.dll")
set_target_properties(Optix_Shared PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Optix_Shared "lib64/optix.6.5.0.lib")
target_include_directories(Optix_Shared PUBLIC "include")
set_target_properties(Optix_Shared PROPERTIES FOLDER "3rdParty/OptiX6.5")

add_library(Optixu_Shared "bin64/optixu.6.5.0.dll")
set_target_properties(Optixu_Shared PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Optixu_Shared "lib64/optixu.6.5.0.lib")
target_include_directories(Optixu_Shared PUBLIC "include")
set_target_properties(Optixu_Shared PROPERTIES FOLDER "3rdParty/OptiX6.5")

find_package(CUDA)
target_link_libraries(Optix_Shared CUDA)
target_link_libraries(Optixu_Shared CUDA)
