#ifndef __RAY_TRACER_HPP__
#define __RAY_TRACER_HPP__

#include "utils.hpp"
#include "mat4.hpp"
#include "ray-tracing-scene.hpp"
#include "ray.hpp"
#include "shape.hpp"
#include "vec3.hpp"

#include <opencv2/opencv.hpp>


/*
        float *getDistances(const vec3 &eye=DEFAULT_EYE, const vec3 &center=DEFAULT_CENTER, const vec3 &up=DEFAULT_UP, std::function<void(int, int)> callback=[](int,int){}) const;

        float *getDistances(const mat4 &camera, std::function<void(int, int)> callback=[](int,int){}) const;
*/
float *trace_scene_raw(const std::string &filename, bool invert=false, bool verbosity=false, std::function<void(int,int)> callback=[](int,int){});

cv::Mat trace_scene(const std::string &filename, bool invert=false, bool verbosity=false, std::function<void(int,int)> callback=[](int,int){});

#endif // __RAY_TRACER_HPP__
