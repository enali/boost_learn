cmake_minimum_required(VERSION 3.10)
project(boost_learn)

set(CMAKE_CXX_STANDARD 14)

set(BOOST_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED)

add_subdirectory(boost_examples)
