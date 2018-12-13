cmake_minimum_required (VERSION 2.6)

set(LIB_BINDINGS_PYTHON "lib_bindings_python")
set(PACKAGE_NAME "sourcetraildb")


# --- Find Python ---

if (NOT EXISTS "${PYTHON_INCLUDE_DIRS}")
	message(STATUS "Python include dir \"${PYTHON_INCLUDE_DIRS}\" does not exist, trying to find Python automatically.")
	find_package(PythonLibs REQUIRED)
else ()
	if (NOT EXISTS "${PYTHON_LIBRARIES}")
		message(STATUS "Python library \"${PYTHON_LIBRARIES}\" does not exist, trying to find Python automatically.")
		find_package(PythonLibs REQUIRED)
	endif ()
endif ()

message(STATUS "Found Python include dirs: ${PYTHON_INCLUDE_DIRS}")
message(STATUS "Found Python libraries: ${PYTHON_LIBRARIES}")


# --- Setup Paths ---

set(RESOURCES_SWIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../resources_swig")
set(GENERATED_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/src")

set(SWIG_INTERFACE_FILE "${RESOURCES_SWIG_DIR}/interface/${PACKAGE_NAME}.i")
set(GENERATED_WRAPPER_FILE "${GENERATED_SRC_DIR}/${PACKAGE_NAME}_wrap.cxx")

file(WRITE ${GENERATED_WRAPPER_FILE} "")

set(SRC_FILES
	"${RESOURCES_SWIG_DIR}/src/${PACKAGE_NAME}.cpp"
	${GENERATED_WRAPPER_FILE}
)

set(HDR_FILES
	"${RESOURCES_SWIG_DIR}/include/${PACKAGE_NAME}.h"
)


# --- Configure Target ---

add_library(${LIB_BINDINGS_PYTHON} SHARED ${SRC_FILES} ${HDR_FILES} ${SWIG_INTERFACE_FILE})

set_target_properties(${LIB_BINDINGS_PYTHON} PROPERTIES OUTPUT_NAME "_${PACKAGE_NAME}")

if (WIN32)
	set_target_properties(${LIB_BINDINGS_PYTHON} PROPERTIES SUFFIX ".pyd")
else()
	# Remove 'lib' prefix
	set_target_properties(${LIB_BINDINGS_PYTHON} PROPERTIES PREFIX "")
	set_target_properties(${LIB_BINDINGS_PYTHON} PROPERTIES SUFFIX ".so")
endif()

if (WIN32)
	add_custom_command(
		TARGET ${LIB_BINDINGS_PYTHON}
		PRE_BUILD
		COMMAND if not exist \"${GENERATED_SRC_DIR}\" mkdir \"${GENERATED_SRC_DIR}\" \n swig -c++ -python -I${RESOURCES_SWIG_DIR}/include -o ${GENERATED_WRAPPER_FILE} -outdir ${CMAKE_CURRENT_BINARY_DIR}/$(CONFIGURATION) ${SWIG_INTERFACE_FILE}
		COMMENT "Generating wrapper code file."
	)
else()
	add_custom_command(
		TARGET ${LIB_BINDINGS_PYTHON}
		PRE_BUILD
		COMMAND swig -c++ -python -I${RESOURCES_SWIG_DIR}/include -o ${GENERATED_WRAPPER_FILE} -outdir ${CMAKE_CURRENT_BINARY_DIR} ${SWIG_INTERFACE_FILE}
		COMMENT "Generating wrapper code file."
	)
endif()


target_include_directories(${LIB_BINDINGS_PYTHON} PUBLIC
	"${RESOURCES_SWIG_DIR}/include"
	"${CORE_SOURCE_DIR}/include"
	${PYTHON_INCLUDE_DIRS}
)

target_link_libraries(${LIB_BINDINGS_PYTHON} ${PYTHON_LIBRARIES} ${LIB_CORE})
