cmake_minimum_required (VERSION 2.6)

set(PACKAGE_NAME "sourcetraildb")

# --- Setup Paths ---

set(RESOURCES_SWIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../resources_swig")
set(GENERATED_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/src")
set(PYTHON_BINDING_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

set(SWIG_INTERFACE_FILE "${RESOURCES_SWIG_DIR}/interface/${PACKAGE_NAME}.i")


# --- Find Python ---

find_package(PythonLibs)


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

swig_add_library(
	${PACKAGE_NAME}
	TYPE SHARED
	LANGUAGE python
	OUTPUT_DIR ${PYTHON_BINDING_OUTPUT_DIR}
	OUTFILE_DIR ${GENERATED_SRC_DIR}
	SOURCES ${SWIG_INTERFACE_FILE} ${RESOURCES_SWIG_DIR}/src/${PACKAGE_NAME}.cpp
)

swig_link_libraries(${PACKAGE_NAME} ${PYTHON_LIBRARIES} ${LIB_CORE})
