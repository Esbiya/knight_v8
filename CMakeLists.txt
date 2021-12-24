cmake_minimum_required(VERSION 3.0.0)
project(rs-crypto VERSION 0.1.0)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/knight_v8)
set(CMAKE_CXX_FLAGS "-std=c++14 -DV8_COMPRESS_POINTERS ${CMAKE_CXX_FLAGS}")

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

add_library(v8py SHARED v8py.cpp)

target_link_libraries(
    v8py
    ${PROJECT_SOURCE_DIR}/libs/dawwin_x86_64/libv8.a
)
