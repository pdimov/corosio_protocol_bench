# Copyright 2026 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.14...4.2)

project(corosio_protocol_bench LANGUAGES CXX)

include(FetchContent)

# Boost::capy

message(STATUS "Fetching Boost::capy")

FetchContent_Declare(capy
    GIT_REPOSITORY https://github.com/cppalliance/capy.git
    GIT_TAG        develop)

FetchContent_MakeAvailable(capy)

# Boost::corosio

message(STATUS "Fetching Boost::corosio")

FetchContent_Declare(corosio
    GIT_REPOSITORY https://github.com/cppalliance/corosio.git
    GIT_TAG        develop)

FetchContent_MakeAvailable(corosio)

#

add_executable(corosio_protocol_bench)

target_sources(corosio_protocol_bench PRIVATE main.cpp)

target_link_libraries(corosio_protocol_bench PRIVATE Boost::capy Boost::corosio)
