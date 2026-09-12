cmake_minimum_required(VERSION 4.2)
project(raylib_made_hero)

set(CMAKE_CXX_STANDARD 11)

find_package(raylib CONFIG REQUIRED)

add_executable(raylib_made_hero main.cpp)

target_link_libraries(raylib_made_hero PRIVATE raylib)
