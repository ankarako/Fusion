# Identifies selected System Compiler
#   \Name: GetSystemCompiler
#   \param[out]: CompilerRet    string containing the compiler Id.
#   
#   Compiler Ids:
#   "Clang" for Clang
#   "GNU" for GCC
#   "MSVC" fro MSVC
function (GetSystemCompiler CompilerRet)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(${CompilerRet} "Clang" PARENT_SCOPE)
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(${CompilerRet} "GNU" PARENT_SCOPE)
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(${CompilerRet} "MSVC" PARENT_SCOPE)
    endif()
endfunction()