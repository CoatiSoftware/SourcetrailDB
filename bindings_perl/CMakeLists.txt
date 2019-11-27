cmake_minimum_required (VERSION 2.6)

# --- Setup Paths ---

set(RESOURCES_SWIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../resources_swig")
set(GENERATED_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/src")
set(PERL_BINDING_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

set(SWIG_INTERFACE_FILE "${RESOURCES_SWIG_DIR}/interface/sourcetraildb.i")


# --- Find Perl ---

find_package(PerlLibs ${PERL_VERSION} REQUIRED)


# --- Find Swig ---

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})


# --- Configure Target ---

set_source_files_properties(${SWIG_INTERFACE_FILE} PROPERTIES CPLUSPLUS ON)

include_directories(
	"${RESOURCES_SWIG_DIR}/include"
	"${CORE_SOURCE_DIR}/include"
	${PERL_INCLUDE_DIRS}
)

swig_add_library(
	${PERL_BINDING_TARGET_NAME}
	TYPE SHARED
	LANGUAGE perl
	OUTPUT_DIR ${PERL_BINDING_OUTPUT_DIR}
	OUTFILE_DIR ${GENERATED_SRC_DIR}
	SOURCES ${SWIG_INTERFACE_FILE} ${RESOURCES_SWIG_DIR}/src/sourcetraildb.cpp
)

set_target_properties(${PERL_BINDING_TARGET_NAME}
                      PROPERTIES OUTPUT_NAME sourcetraildb)

swig_link_libraries(${PERL_BINDING_TARGET_NAME} ${PERL_LIBRARIES} ${LIB_CORE_TARGET_NAME})
