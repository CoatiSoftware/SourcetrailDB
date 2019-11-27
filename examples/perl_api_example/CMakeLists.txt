cmake_minimum_required (VERSION 2.6)

set(EXAMPLE_NAME "perl_api_example")

# --- Configure Sourcetrail Project File ---

add_custom_target(
	${EXAMPLE_NAME} ALL
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/example.pl ${CMAKE_CURRENT_BINARY_DIR}/example.pl
)

add_dependencies(${EXAMPLE_NAME} _${PERL_BINDING_TARGET_NAME})

if (MSVC)
	STRING(REGEX REPLACE "/" "\\\\" BACKSLASHED_PERL_BINDING_OUTPUT_DIR ${PERL_BINDING_OUTPUT_DIR})
	STRING(REGEX REPLACE "/" "\\\\" BACKSLASHED_CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

	add_custom_command(
		TARGET ${EXAMPLE_NAME} POST_BUILD
		COMMAND xcopy \"${BACKSLASHED_PERL_BINDING_OUTPUT_DIR}\\$(Configuration)\\sourcetraildb*\" \"${BACKSLASHED_CMAKE_CURRENT_BINARY_DIR}\" /Y
	)
	add_custom_command(
		TARGET ${EXAMPLE_NAME} POST_BUILD
		COMMAND xcopy \"${BACKSLASHED_PERL_BINDING_OUTPUT_DIR}\\sourcetraildb.pm\" \"${BACKSLASHED_CMAKE_CURRENT_BINARY_DIR}\" /Y
	)
else()
	add_custom_command(
		TARGET ${EXAMPLE_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${PERL_BINDING_OUTPUT_DIR}/sourcetraildb* ${CMAKE_CURRENT_BINARY_DIR}
	)

	if (APPLE)
		add_custom_command(
			TARGET ${EXAMPLE_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_BINARY_DIR}/sourcetraildb.dylib ${CMAKE_CURRENT_BINARY_DIR}/sourcetraildb.so
		)
	endif()
endif()


set(PERL_API_EXAMPLE_DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME}.srctrlprj.in"
	"${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE_NAME}.srctrlprj"
)
