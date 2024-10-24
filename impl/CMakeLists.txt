set (libName "json_parser")

file(GLOB src_files *.cpp *.h *.hpp *.hh)
add_library(${libName} STATIC ${src_files})

set (BOOST_PROP_TREE_WRITE_FILE "${Boost_INCLUDE_DIRS}/boost/property_tree/json_parser/detail/write.hpp")
set (WRITE_FIX_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../boost_fix/write.hpp")

add_custom_command(
        TARGET ${libName} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${WRITE_FIX_FILE}
        ${BOOST_PROP_TREE_WRITE_FILE}
)

if(NOT MSVC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fcoroutines -std=c++20")    
endif(NOT MSVC)

message("========== The boost include directory was found at ${Boost_INCLUDE_DIRS}")
target_include_directories(${libName} PUBLIC .)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/.. 
  ${Boost_INCLUDE_DIRS}
  SYSTEM
)
