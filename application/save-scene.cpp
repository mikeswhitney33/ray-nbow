#include "../ray-tracer/ray-tracer.hpp"
#include <opencv2/opencv.hpp>
#include <string>

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <scene filename>" << std::endl;
        return -1;
    }
    std::string outfile = "out.png";
    if(argc >= 3)
    {
        outfile = std::string(argv[2]);
    }
    std::string filename(argv[1]);

    cv::Mat im = trace_scene(filename, true);
    
    cv::Mat display;
    im.convertTo(display, CV_8UC1, 255);
    cv::imwrite(outfile, display);
    return 0;
}
