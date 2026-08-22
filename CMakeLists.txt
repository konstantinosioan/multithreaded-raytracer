cmake_minimum_required(VERSION 3.20)
project(multithreaded-raytracer LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(raytracer src/main.cpp)

target_compile_options(raytracer PRIVATE
    -Wall -Wextra -Wconversion -Wsign-conversion -Wshadow
    -Wold-style-cast -pedantic-errors)
