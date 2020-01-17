cmake_minimum_required(VERSION 3.0.0)

find_package(OpenCV REQUIRED)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(ray-nbow)

enable_testing()

add_subdirectory(ray-tracer)
add_subdirectory(application)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/)
