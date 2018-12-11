cmake_minimum_required (VERSION 2.6)

set(PROJECT_NAME "SourcetrailDB")

project(${PROJECT_NAME})

# configure default build type to Release
set(CMAKE_BUILD_TYPE_INIT "Release")


set(LIB_CORE "lib_core")
set(CORE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/core") # can be accessed in subdirectory CMake files
set(CORE_BINARY_DIR "${CMAKE_BINARY_DIR}/core") # can be accessed in subdirectory CMake files

add_subdirectory(${CORE_SOURCE_DIR} ${CORE_BINARY_DIR})
add_subdirectory("${CMAKE_SOURCE_DIR}/bindings_python" "${CMAKE_BINARY_DIR}/bindings_python")
