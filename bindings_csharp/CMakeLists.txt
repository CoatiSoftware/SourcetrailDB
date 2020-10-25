cmake_minimum_required (VERSION 2.6)

# --- Setup Paths ---

set(RESOURCES_SWIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../resources_swig")
set(GENERATED_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/src")
set(CSHARP_BINDING_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

set(SWIG_INTERFACE_FILE "${RESOURCES_SWIG_DIR}/interface/sourcetraildb.i")
set(CSHARP_BINDINGS_NAMESPACE "CoatiSoftware.SourcetrailDB")
set(CSHARP_BINDING_TARGET_OUTPUT_NAME "SourcetrailDB")

# --- Find Swig ---

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

# --- Configure Target ---

set_source_files_properties(${SWIG_INTERFACE_FILE} PROPERTIES CPLUSPLUS ON)
set_property(SOURCE ${SWIG_INTERFACE_FILE} PROPERTY COMPILE_OPTIONS -namespace ${CSHARP_BINDINGS_NAMESPACE} -dllimport ${CSHARP_BINDING_TARGET_OUTPUT_NAME})

include_directories(
	"${RESOURCES_SWIG_DIR}/include"
	"${CORE_SOURCE_DIR}/include"
)

swig_add_library(
	${CSHARP_BINDING_TARGET_NAME}
	TYPE SHARED
	LANGUAGE csharp
	OUTPUT_DIR ${CSHARP_BINDING_OUTPUT_DIR}
	OUTFILE_DIR ${GENERATED_SRC_DIR}
	SOURCES ${SWIG_INTERFACE_FILE} ${RESOURCES_SWIG_DIR}/src/sourcetraildb.cpp
)

set_target_properties(${CSHARP_BINDING_TARGET_NAME}
                      PROPERTIES OUTPUT_NAME ${CSHARP_BINDING_TARGET_OUTPUT_NAME}
	SWIG_FLAGS ${CSHARP_BINDINGS_NAMESPACE})

swig_link_libraries(${CSHARP_BINDING_TARGET_NAME} ${CSHARP_LIBRARIES} ${LIB_CORE_TARGET_NAME})

configure_file("CoatiSoftware.SourcetrailDB.csproj" "CoatiSoftware.SourcetrailDB.csproj" COPYONLY)