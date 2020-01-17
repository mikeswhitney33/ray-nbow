cmake_minimum_required(VERSION 3.0.0)


set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(ray-nbow)

find_package(OpenCV REQUIRED)

enable_testing()

link_libraries(pthread)
link_libraries(${OpenCV_LIBS})
include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(ray-tracer)

link_libraries(ray-tracer)
add_subdirectory(application)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/)
