cmake_minimum_required(VERSION 3.23.0)
project(json_parser)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)


include_directories("${PROJECT_BINARY_DIR}")

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)
message("------------------------- Our boost is found at ${Boost_INCLUDE_DIRS} --------------------------")
include_directories(${Boost_INCLUDE_DIRS} SYSTEM)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
include(CPack)
add_subdirectory(impl)
add_subdirectory(examples)

# test support
enable_testing()