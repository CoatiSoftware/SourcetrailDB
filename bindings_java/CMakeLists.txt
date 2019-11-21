cmake_minimum_required (VERSION 2.6)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_MACOSX_RPATH 1)
endif()

# --- Setup Paths ---

set(RESOURCES_SWIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../resources_swig")
set(GENERATED_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/src")
set(JAVA_BINDING_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

set(SWIG_INTERFACE_FILE "${RESOURCES_SWIG_DIR}/interface/sourcetraildb.i")

set(CMAKE_SWIG_FLAGS -package com.sourcetrail)

# --- Find Java ---

find_package(Java REQUIRED COMPONENTS Runtime Development)
find_package(JNI REQUIRED)
include(UseJava)

# --- Find Swig ---

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})


# --- Configure Target ---

set_source_files_properties(${SWIG_INTERFACE_FILE} PROPERTIES CPLUSPLUS ON)

include_directories(
	"${RESOURCES_SWIG_DIR}/include"
	"${CORE_SOURCE_DIR}/include"
	${JNI_INCLUDE_DIRS}
)

swig_add_library(
	${JAVA_BINDING_TARGET_NAME}
	TYPE SHARED
	LANGUAGE java
	OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/com/sourcetrail
	OUTFILE_DIR ${GENERATED_SRC_DIR}
	SOURCES ${SWIG_INTERFACE_FILE} ${RESOURCES_SWIG_DIR}/src/sourcetraildb.cpp
)

swig_link_libraries(${JAVA_BINDING_TARGET_NAME} ${JNI_LIBRARIES} ${LIB_CORE_TARGET_NAME})

set_target_properties(${JAVA_BINDING_TARGET_NAME}
                      PROPERTIES OUTPUT_NAME _sourcetraildb)


set(CMAKE_JNI_TARGET TRUE)
add_jar(
    SourcetrailDB
    SOURCES
		${CMAKE_CURRENT_BINARY_DIR}/com/sourcetrail/DefinitionKind.java
		${CMAKE_CURRENT_BINARY_DIR}/com/sourcetrail/ReferenceKind.java
		${CMAKE_CURRENT_BINARY_DIR}/com/sourcetrail/sourcetraildb.java
		${CMAKE_CURRENT_BINARY_DIR}/com/sourcetrail/sourcetraildbJNI.java
		${CMAKE_CURRENT_BINARY_DIR}/com/sourcetrail/SymbolKind.java
)

add_dependencies( SourcetrailDB ${JAVA_BINDING_TARGET_NAME} )
