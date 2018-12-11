cmake_minimum_required (VERSION 2.6)

set(EXAMPLE_NAME "python_binding_example")

# --- Configure Sourcetrail Project File ---

add_custom_target(
	${EXAMPLE_NAME} ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/example.py ${CMAKE_CURRENT_BINARY_DIR}/example.py
)

add_custom_command(
	TARGET ${EXAMPLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${PYTHON_BINDING_OUTPUT_DIR}/_sourcetraildb* ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${PYTHON_BINDING_OUTPUT_DIR}/sourcetraildb.py ${CMAKE_CURRENT_BINARY_DIR}
)

if (APPLE)
	add_custom_command(
		TARGET ${EXAMPLE_NAME} POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_BINARY_DIR}/_sourcetraildb.dylib ${CMAKE_CURRENT_BINARY_DIR}/_sourcetraildb.so
	)
endif()

set(PYTHON_BINDING_EXAMPLE_DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME}.srctrlprj.in"
	"${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE_NAME}.srctrlprj"
)
