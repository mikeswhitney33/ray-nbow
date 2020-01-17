add_executable(trace-scene trace-scene.cpp)

target_link_libraries(trace-scene ray-tracer ${OpenCV_LIBS})
target_include_directories(trace-scene PUBLIC ${OpenCV_INCLUDE_DIRS})