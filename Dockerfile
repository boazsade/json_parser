FROM ubuntu:latest

RUN apt-get -y update
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install tzdata
ENV LANG en_US.utf8

RUN apt-get install -y build-essential zlib1g \
    zlib1g-dev cmake libgtest-dev ninja-build \
    libssl-dev libboost-all-dev clang-tidy

RUN cp boost_fix/write.hpp /usr/include/boost/property_tree/json_parser/detail/write.hpp 
RUN mkdir /workspace
