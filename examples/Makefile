ifeq ($(BOOST_INCLUDE_PATH),)
    $(error "the location to boost is missing, you must set BOOST_INCLUDE_PATH environment variable to point to boost path")
endif
ifeq ($(BOOST_LIB_PATH),)
    $(error "the location to boost lib is missing, you must set BOOST_LIB_PATH environment variable to point to boost path")
endif

INCLUDE_DIR=$(shell dirname $(PWD))/
#$(error "the include dir is $(BOOST_LIB_PATH)")

JSON_SRC := $(wildcard ../impl/*.cpp)
JSON_OBJS := $(JSON_SRC:.cpp=.o)
SRC_EXAMPLE := struct_example.cpp
OBJ_EXAMPLE := $(SRC_EXAMPLE:.cpp=.o)
SRC := $(JSON_SRC) $(SRC_EXAMPLE)
OBJ := $(JSON_OBJS) $(OBJ_EXAMPLE)
CXXFLAGS += -Wall -g -std=c++17 -isystem $(BOOST_PATH) -I$(INCLUDE_DIR)
LDFLAGS += -L$(BOOST_LIB_PATH) -lboost_system

.PHONY: struct_example
.PHONY: clean

all: struct_example

struct_example: $(OBJ)
	 @$(CXX) $(CFLAGS) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o $@ -c $<

%.o: ../src/%.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o $@ -c $<

clean:
	@rm -rf *.o struct_example ../src/*.o
