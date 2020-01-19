#include "ray-tracer.hpp"


float * trace_scene_raw(const std::string &filename, int &width, int &height, bool invert, bool verbosity, std::function<void(int, int)> callback)
{
    rt::RayTracingScene scene = rt::RayTracingScene::FromScene(filename);
    scene.setVerbosity(verbosity);
    float *pix = scene.getDistances(callback);
    int size = scene.getDims();
    pix = rt::normalize(pix, size, invert);
    width = scene.getWidth();
    height = scene.getHeight();
    return pix;
}

float * trace_scene_raw(const std::string &filename, bool invert, bool verbosity, std::function<void(int, int)> callback)
{
    int width, height;
    return trace_scene_raw(filename, width, height, invert, verbosity, callback);
}

cv::Mat trace_scene(const std::string &filename, bool invert, bool verbosity, std::function<void(int, int)> callback)
{
    int width, height;
    float *pix = trace_scene_raw(filename, width, height, invert, verbosity, callback);
    cv::Mat im(height, width, CV_32FC1, pix);
    cv::Mat res = im.clone();
    delete [] pix;
    return res;
}