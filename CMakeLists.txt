get_filename_component(libName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message("===== Libs: '${libName}'")

file(GLOB src_files *.cpp *.h *.hpp *.hh)
add_library(${libName} STATIC ${src_files})

if(NOT MSVC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fcoroutines -std=c++20")    
endif(NOT MSVC)

set (BOOST_PROP_TREE_WRITE_FILE "${Boost_INCLUDE_DIRS}/boost/property_tree/json_parser/detail/write.hpp")
set (WRITE_FIX_FILE "${CMAKE_CURRENT_SOURCE_DIR}/boost_fix/write.hpp")

add_subdirectory(examples)
add_subdirectory(impl)
find_package(Boost REQUIRED)

add_custom_command(
        TARGET ${libName} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${WRITE_FIX_FILE}
        ${BOOST_PROP_TREE_WRITE_FILE}
)

target_include_directories(${libName} PUBLIC .)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/.. 
  ${Boost_INCLUDE_DIRS}
  Boost::headers
)

# test support
enable_testing()
