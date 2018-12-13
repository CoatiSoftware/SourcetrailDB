cmake_minimum_required (VERSION 2.6)

set(BUILD_BINDINGS_PYTHON OFF CACHE BOOL "Build the SourcetrailDB Python bindings.")

set(PROJECT_NAME "SourcetrailDB")

project(${PROJECT_NAME})

# configure default flags
set(CMAKE_BUILD_TYPE_INIT "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(LIB_CORE "lib_core")
set(CORE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/core") # can be accessed in subdirectory CMake files
set(CORE_BINARY_DIR "${CMAKE_BINARY_DIR}/core") # can be accessed in subdirectory CMake files

add_subdirectory(${CORE_SOURCE_DIR} ${CORE_BINARY_DIR})

if (BUILD_BINDINGS_PYTHON)
	message(STATUS "SourcetrailDB Python bindings will be built.")
	add_subdirectory("${CMAKE_SOURCE_DIR}/bindings_python" "${CMAKE_BINARY_DIR}/bindings_python")
else()
	message(STATUS "Building SourcetrailDB Python bindings will be skipped. You can enable building this target by setting 'BUILD_BINDINGS_PYTHON' to 'ON'.")
endif()
