include(flags)
include(dependencies)

add_library(json_parser STATIC           
           json_base.h json_base.cpp
           json_fwd.h json_stream.h
           json_istream.h json_ostream.h
           json_reader.h json_reader.cpp
           json_writer.h json_writer.cpp
           jsonfwrd.h
	    )


target_include_directories(json_parser PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../)

enable_testing()
