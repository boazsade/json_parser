include(flags)
include(dependencies)

list(APPEND MAIN_FILES
    create_json.cpp
)

add_executable(create_json  ${MAIN_FILES})
list(APPEND EXTRA_LIBS  json_parser Boost::boost)
list(APPEND EXTRA_INCLUDES $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> )

target_include_directories(create_json PUBLIC ${EXTRA_INCLUDES})
target_link_libraries(create_json PUBLIC ${EXTRA_LIBS})