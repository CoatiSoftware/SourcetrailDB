cmake_minimum_required (VERSION 2.6)

# --- Options ---

set(BUILD_BINDINGS_PYTHON OFF CACHE BOOL "Build the SourcetrailDB Python bindings.")
set(BUILD_EXAMPLES ON CACHE BOOL "Build the examples.")

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
    set(GIT_VERSION_NUMBER "")
endif()

if("${GIT_VERSION_NUMBER}" STREQUAL "")
	message(WARNING "Unable to derive SourcetrailDB version number from most recent Git tag.")
    set(GIT_VERSION_NUMBER "v0.db0.p0-0-a")
endif()

string(REGEX REPLACE "v([0-9]+)\\..*" "\\1" INTERFACE_VERSION "${GIT_VERSION_NUMBER}")
string(REGEX REPLACE "v[0-9]+\\.db([0-9]+)\\..*" "\\1" DATABASE_VERSION "${GIT_VERSION_NUMBER}")
string(REGEX REPLACE "v[0-9]+\\.db[0-9]+\\.p([0-9]+)-.*" "\\1" PATCH_VERSION "${GIT_VERSION_NUMBER}")
set(VERSION_STRING "v${INTERFACE_VERSION}.db${DATABASE_VERSION}.p${PATCH_VERSION}")
set(VERSION_STRING_UNDERSCORE "v${INTERFACE_VERSION}_db${DATABASE_VERSION}_p${PATCH_VERSION}")

message(STATUS "SourcetrailDB Version: ${VERSION_STRING}")
message(STATUS "Interface Version: ${INTERFACE_VERSION}")
message(STATUS "Database Version: ${DATABASE_VERSION}")
message(STATUS "Patch Version: ${PATCH_VERSION}")

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/version.txt.in
	"${CMAKE_CURRENT_BINARY_DIR}/version.txt"
)


# --- Core ---

set(LIB_CORE "lib_core")
set(CORE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/core") # can be accessed in subdirectory CMake files
set(CORE_BINARY_DIR "${CMAKE_BINARY_DIR}/core") # can be accessed in subdirectory CMake files

add_subdirectory(${CORE_SOURCE_DIR} ${CORE_BINARY_DIR})


# --- Python Binding ---

if (BUILD_BINDINGS_PYTHON)
	message(STATUS "The SourcetrailDB Python bindings will be built.")

	set(PYTHON_BINDING_OUTPUT_DIR "")
	add_subdirectory("${CMAKE_SOURCE_DIR}/bindings_python" "${CMAKE_BINARY_DIR}/bindings_python")
else()
	message(STATUS "Building the SourcetrailDB Python bindings will be skipped. You can enable building this target by setting 'BUILD_BINDINGS_PYTHON' to 'ON'.")
endif()


# --- Examples ---

if (BUILD_EXAMPLES)
	message(STATUS "The examples will be built.")
	add_subdirectory("${CMAKE_SOURCE_DIR}/examples/cpp_api_example")
	add_subdirectory("${CMAKE_SOURCE_DIR}/examples/cpp_poetry_indexer")

	if (BUILD_BINDINGS_PYTHON)
		add_subdirectory("${CMAKE_SOURCE_DIR}/examples/python_api_example")
	else()
		message(STATUS "Skip python binding sample. You can enable building it by setting 'BUILD_BINDINGS_PYTHON' to 'ON'.")
	endif()
else()
	message(STATUS "Building examples will be skipped. You can enable building examples by setting 'BUILD_EXAMPLES' to 'ON'.")
endif()
