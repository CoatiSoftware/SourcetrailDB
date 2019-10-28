cmake_minimum_required (VERSION 2.6)

# --- Setup Paths ---

set(RESOURCES_SWIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../resources_swig")
set(GENERATED_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/src")
set(PYTHON_BINDING_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

set(SWIG_INTERFACE_FILE "${RESOURCES_SWIG_DIR}/interface/sourcetraildb.i")


# --- Find Python ---

find_package(PythonLibs ${PYTHON_VERSION} REQUIRED)


# --- Find Swig ---

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})


# --- Configure Target ---

set_source_files_properties(${SWIG_INTERFACE_FILE} PROPERTIES CPLUSPLUS ON)

include_directories(
	"${RESOURCES_SWIG_DIR}/include"
	"${CORE_SOURCE_DIR}/include"
	${PYTHON_INCLUDE_DIRS}
)

set (UseSWIG_TARGET_NAME_PREFERENCE STANDARD)

swig_add_library(
	${PYTHON_BINDING_TARGET_NAME}
	TYPE SHARED
	LANGUAGE python
	OUTPUT_DIR ${PYTHON_BINDING_OUTPUT_DIR}
	OUTFILE_DIR ${GENERATED_SRC_DIR}
	SOURCES ${SWIG_INTERFACE_FILE} ${RESOURCES_SWIG_DIR}/src/sourcetraildb.cpp
)

set_target_properties(${PYTHON_BINDING_TARGET_NAME}
                      PROPERTIES OUTPUT_NAME sourcetraildb)

TARGET_LINK_LIBRARIES(${PYTHON_BINDING_TARGET_NAME} ${PYTHON_LIBRARIES} ${LIB_CORE_TARGET_NAME})
