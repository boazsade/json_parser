include(flags)
include(dependencies)

list(APPEND MAIN_FILES
    struct_example.cpp
)

add_executable(struct_example  ${MAIN_FILES})
list(APPEND EXTRA_LIBS  json_parser)
list(APPEND EXTRA_INCLUDES $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> )

target_include_directories(struct_example PUBLIC ${EXTRA_INCLUDES})
target_link_libraries(struct_example PUBLIC ${EXTRA_LIBS})
include_directories(${Boost_INCLUDE_DIRS} SYSTEM)