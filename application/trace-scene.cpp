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
	rt::RayTracingScene scene = rt::RayTracingScene::FromScene(argv[1]);
	
	std::cout << "Tracing " << scene.size() << " shapes" << std::endl;
	Timer timer;
	timer.reset();
	float *pix = scene.getDistances({0, 0, -1}, {0, 0, 0}, {0, 1, 0}, [&](int k, int total)
	{
		std::cout << k << "/" << total << " -- " << timer << std::endl;
	});
	std::cout << timer << std::endl;
	int size = scene.getDims();
	pix = rt::normalize(pix, size);
	unsigned char *data = rt::touchar(pix, size);
	cv::Mat im(scene.getHeight(), scene.getWidth(), CV_8UC1, data);

	cv::imshow("im", im);
	cv::waitKey(0);

	delete [] pix;
	delete [] data;
	return 0;
}
