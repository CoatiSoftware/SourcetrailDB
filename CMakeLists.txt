cmake_minimum_required (VERSION 2.6)

set(BUILD_BINDINGS_PYTHON OFF CACHE BOOL "Build the SourcetrailDB Python bindings.")

set(PROJECT_NAME "SourcetrailDB")

project(${PROJECT_NAME})


# --- Default Flags ---

set(CMAKE_BUILD_TYPE_INIT "Release")

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -fPIC")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -fPIC")
endif()


# --- Version ---

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
	find_package(Git)

    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --long
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_VERSION_NUMBER
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    set(GIT_VERSION_NUMBER "v0_db0_p0-0-a")
endif()

string(REGEX REPLACE "v([0-9]+)_.*" "\\1" INTERFACE_VERSION "${GIT_VERSION_NUMBER}")
string(REGEX REPLACE "v[0-9]+_db([0-9]+)_.*" "\\1" DATABASE_VERSION "${GIT_VERSION_NUMBER}")
string(REGEX REPLACE "v[0-9]+_db[0-9]+_p([0-9])-.*" "\\1" PATCH_VERSION "${GIT_VERSION_NUMBER}")
set(VERSION_STRING "v${INTERFACE_VERSION}_db${DATABASE_VERSION}_p${PATCH_VERSION}")

message(STATUS ${INTERFACE_VERSION})
message(STATUS ${DATABASE_VERSION})
message(STATUS ${PATCH_VERSION})
message(STATUS ${VERSION_STRING})

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/version.txt.in
	"${CMAKE_CURRENT_BINARY_DIR}/version.txt"
)



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
