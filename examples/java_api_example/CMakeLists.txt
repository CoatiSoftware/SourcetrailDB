cmake_minimum_required (VERSION 2.6)

# --- Find Java ---

find_package(Java REQUIRED COMPONENTS Runtime Development)
include(UseJava)

set(EXAMPLE_NAME "java_api_example")

# --- Configure Sourcetrail Project File ---

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/za/co/spazzymoto/example.java ${CMAKE_CURRENT_BINARY_DIR}/za/co/spazzymoto/example.java COPYONLY)

add_custom_target(
	CopyJar ALL
	COMMAND ${CMAKE_COMMAND} -E copy ${JAVA_BINDING_OUTPUT_DIR}/SourcetrailDB.jar ${CMAKE_CURRENT_BINARY_DIR}/SourcetrailDB.jar
)

add_jar(
	${EXAMPLE_NAME}
	SOURCES 
	${CMAKE_CURRENT_BINARY_DIR}/za/co/spazzymoto/example.java
	INCLUDE_JARS
	${CMAKE_CURRENT_BINARY_DIR}/SourcetrailDB.jar
	ENTRY_POINT za.co.spazzymoto.example
)

add_dependencies( ${EXAMPLE_NAME} CopyJar )
add_dependencies( CopyJar ${JAVA_BINDING_TARGET_NAME} )

if (MSVC)
	STRING(REGEX REPLACE "/" "\\\\" BACKSLASHED_JAVA_BINDING_OUTPUT_DIR ${JAVA_BINDING_OUTPUT_DIR})
	STRING(REGEX REPLACE "/" "\\\\" BACKSLASHED_CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

	add_custom_command(
		TARGET ${EXAMPLE_NAME} POST_BUILD
		COMMAND xcopy \"${BACKSLASHED_JAVA_BINDING_OUTPUT_DIR}\\$(Configuration)\\_sourcetraildb*\" \"${BACKSLASHED_CMAKE_CURRENT_BINARY_DIR}\" /Y
	)
else()
	add_custom_command(
		TARGET ${EXAMPLE_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${JAVA_BINDING_OUTPUT_DIR}/lib_sourcetraildb* ${CMAKE_CURRENT_BINARY_DIR}
	)
endif()

set(JAVA_API_EXAMPLE_DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME}.srctrlprj.in"
	"${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE_NAME}.srctrlprj"
)
