cmake_minimum_required (VERSION 2.6)

set(PROJECT_NAME "SourcetrailDBBindingsPython")
set(PACKAGE_NAME "sourcetraildb")
set(LIB_NAME "_${PACKAGE_NAME}")

project(${PROJECT_NAME})

# check for current architecture
if (NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
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

set(RESOURCES_SWIG_DIR "${CMAKE_SOURCE_DIR}/../resources_swig")
set(GENERATED_SRC_DIR "${CMAKE_BINARY_DIR}/src")

set(SWIG_INTERFACE_FILE "${RESOURCES_SWIG_DIR}/interface/${PACKAGE_NAME}.i")
set(GENERATED_WRAPPER_FILE "${GENERATED_SRC_DIR}/${PACKAGE_NAME}_wrap.cxx")

set(CORE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../core/include" CACHE FILEPATH "path to core include directory")
set(CORE_DEBUG_LIBRARY "${CMAKE_SOURCE_DIR}/../core/build/${ARCH}bit/Debug/sourcetraildb.lib" CACHE FILEPATH "path to core library directory")
set(CORE_RELEASE_LIBRARY "${CMAKE_SOURCE_DIR}/../core/build/${ARCH}bit/Release/sourcetraildb.lib" CACHE FILEPATH "path to core library directory")

if (WIN32)
	file(WRITE ${GENERATED_WRAPPER_FILE} "")
endif ()

set(SRC_FILES 
	"${RESOURCES_SWIG_DIR}/src/${PACKAGE_NAME}.cpp"
	${GENERATED_WRAPPER_FILE}
)

set(HDR_FILES 
	"${RESOURCES_SWIG_DIR}/include/${PACKAGE_NAME}.h"
)


# --- Configure Target ---

add_library(${LIB_NAME} SHARED ${SRC_FILES} ${HDR_FILES} ${SWIG_INTERFACE_FILE})

set_target_properties(${LIB_NAME} PROPERTIES SUFFIX ".pyd")

add_custom_command(
	TARGET ${LIB_NAME}
	PRE_BUILD
	COMMAND if not exist \"${GENERATED_SRC_DIR}\" mkdir \"${GENERATED_SRC_DIR}\" \n $ENV{SWIG_DIR}/swig.exe -c++ -python -I${RESOURCES_SWIG_DIR}/include -o ${GENERATED_WRAPPER_FILE} -outdir ${CMAKE_BINARY_DIR}/$(CONFIGURATION) ${SWIG_INTERFACE_FILE}
	COMMENT "Generating wrapper code file."
)

target_include_directories(${LIB_NAME} PUBLIC
	"${RESOURCES_SWIG_DIR}/include"
	"${CORE_INCLUDE_DIR}"
	${PYTHON_INCLUDE_DIRS}
)

message("CORE_RELEASE_LIBRARY: ${CORE_RELEASE_LIBRARY}")

target_link_libraries(${LIB_NAME} ${PYTHON_LIBRARIES})
target_link_libraries(${LIB_NAME} debug "${CORE_DEBUG_LIBRARY}" )
target_link_libraries(${LIB_NAME} optimized "${CORE_RELEASE_LIBRARY}" )
