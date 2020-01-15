add_executable(trace-tester test-ray-tracer.cpp)
target_link_libraries(trace-tester ray-tracer)


add_test(test_ray_tracer trace-tester test.ppm)
target_compile_definitions(trace-tester PUBLIC DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test_data")