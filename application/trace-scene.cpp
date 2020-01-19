#include "../ray-tracer/ray-tracer.hpp"
#include <opencv2/opencv.hpp>
#include "timer.h"
#include <string>

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
	
	cv::Mat display;
	im.convertTo(display, CV_8UC1, 255);
	cv::imshow("display", display);
	cv::waitKey(0);
	return 0;
}
