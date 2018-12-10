cmake_minimum_required (VERSION 2.6)

set(PROJECT_NAME "SourcetrailDBCore")
set(LIB_NAME "sourcetraildb")
set(TEST_NAME "test")

set(INTERFACE_VERSION 0)
set(DATABASE_VERSION 22)
set(COMMIT_VERSION 1)
set(VERSION_STRING "v${INTERFACE_VERSION}_db${DATABASE_VERSION}_c${COMMIT_VERSION}")

project(${PROJECT_NAME})

# check for current architecture
if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
    set(ARCH 32)
else()
    set(ARCH 64)
endif()

# configure default build type to Release
set(CMAKE_BUILD_TYPE_INIT "Release")

# configure the output directory
if (UNIX)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${ARCH}bit/${CMAKE_BUILD_TYPE}/")
else ()
	foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
		string( TOUPPER ${OUTPUTCONFIG} UPPER_OUTPUTCONFIG )
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${UPPER_OUTPUTCONFIG} "${CMAKE_SOURCE_DIR}/build/${ARCH}bit/${OUTPUTCONFIG}/")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${UPPER_OUTPUTCONFIG} "${CMAKE_SOURCE_DIR}/build/${ARCH}bit/${OUTPUTCONFIG}/")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${UPPER_OUTPUTCONFIG} "${CMAKE_SOURCE_DIR}/build/${ARCH}bit/${OUTPUTCONFIG}/")
	endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
endif ()


set(GENERATED_INCLUDE_DIRECTORY "${CMAKE_BINARY_DIR}/include")
set(GENERATED_VERSION_FILE "${GENERATED_INCLUDE_DIRECTORY}/version.h")

configure_file(
	${CMAKE_SOURCE_DIR}/version.h.in
	${GENERATED_VERSION_FILE}
)

set(LIB_SRC_FILES 
	external/sqlite/CppSQLite3.cpp
	external/sqlite/sqlite3.c
	src/DatabaseStorage.cpp
	src/DefinitionKind.cpp
	src/EdgeKind.cpp
	src/LocationKind.cpp
	src/NameHierarchy.cpp
	src/NodeKind.cpp
	src/ReferenceKind.cpp
	src/SourcetrailDBWriter.cpp
	src/SymbolKind.cpp
	src/utility.cpp
)

set(LIB_HDR_FILES 
	external/json/json.hpp
	external/sqlite/CppSQLite3.h
	external/sqlite/sqlite3.h
	include/DatabaseStorage.h
	include/DefinitionKind.h
	include/EdgeKind.h
	include/LocationKind.h
	include/NameElement.h
	include/NameHierarchy.h
	include/NodeKind.h
	include/ReferenceKind.h
	include/SourceLocation.h
	include/SourceRange.h
	include/SourcetrailDBWriter.h
	include/SourcetrailException.h
	include/StorageEdge.h
	include/StorageError.h
	include/StorageFile.h
	include/StorageLocalSymbol.h
	include/StorageNode.h
	include/StorageOccurrence.h
	include/StorageSourceLocation.h
	include/StorageSymbol.h
	include/SymbolKind.h
	include/utility.h
	${GENERATED_VERSION_FILE}
)

add_library(${LIB_NAME} STATIC ${LIB_SRC_FILES} ${LIB_HDR_FILES})

target_include_directories(${LIB_NAME} PUBLIC
	"${CMAKE_SOURCE_DIR}/include"
	"${CMAKE_SOURCE_DIR}/external"
	"${GENERATED_INCLUDE_DIRECTORY}"
)


set(TEST_SRC_FILES 
	test/test.cpp
)

add_executable(${TEST_NAME} ${TEST_SRC_FILES})

target_include_directories(${TEST_NAME} PUBLIC
	"${CMAKE_SOURCE_DIR}/include"
	"${CMAKE_SOURCE_DIR}/external"
	"${GENERATED_INCLUDE_DIRECTORY}"
)

target_link_libraries(${TEST_NAME} ${LIB_NAME})
