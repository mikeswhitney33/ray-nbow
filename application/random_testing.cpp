#include <iostream>
#include <opencv2/opencv.hpp>
#include "rtrandom.h"
#include "timer.h"
#include "../ray-tracer/ray-tracer.hpp"

int main(int argc, char ** argv)
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <scene filename>" << std::endl;
        return -1;
    }
    std::string filename(argv[1]);

    Timer timer;
    cv::Mat im = trace_scene(filename, true);
    std::cout << "Traced in: " << timer << std::endl;
    cv::Mat bgr;
    cv::cvtColor(im, bgr, cv::COLOR_GRAY2BGR);

    RTRandom random(0, 1);
    rt::vec3 color = {random.get(), random.get(), random.get()};
    for(int r = 0;r < im.rows;r++)
    {
        for(int c = 0;c < im.cols;c++)
        {
            cv::Vec3f &pix = bgr.at<cv::Vec3f>(r, c);
            pix[0] *= color.x;
            pix[1] *= color.y;
            pix[2] *= color.z;
        }
    }

    cv::Mat display;
    bgr.convertTo(display, CV_8UC3, 255);
    cv::imshow("display", display);
    cv::waitKey(0);
    return 0;
}