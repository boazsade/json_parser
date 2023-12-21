include(flags)
include(dependencies)

list(APPEND MAIN_FILES
    read_json.cpp
)

add_executable(read_json  ${MAIN_FILES})
list(APPEND EXTRA_LIBS  json_parser)
list(APPEND EXTRA_INCLUDES $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> )

target_include_directories(read_json PUBLIC ${EXTRA_INCLUDES})
target_link_libraries(read_json PUBLIC ${EXTRA_LIBS})