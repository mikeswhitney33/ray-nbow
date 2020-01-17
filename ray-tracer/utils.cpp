#include "utils.hpp"
#include <limits>

#include <iostream>
#include <cmath>

#include <fstream>

namespace rt
{
    float radians(const float &deg)
    {
        return deg * M_PI / 180.0f;
    }

    unsigned char touchar(const float &f)
    {
        return (f < 0 ? 0 : f > 1 ? 1 : f) * 255.0f;
    }

    float *normalize(float *pix, const int &size)
    {
    	float minval = std::numeric_limits<float>::max(), maxval = 0;
    	for(int i = 0;i < size;i++)
    	{
    		float p = pix[i];
    		if(p >= 0 && p < minval)
    		{
    			minval = p;
    		}
    		if(p > maxval)
    		{
    			maxval = p;
    		}
    	}
    	for(int i =0;i < size;i++)
    	{
    		pix[i] = (pix[i] - minval) / (maxval - minval);
    	}
    	std::cout << "Normalized (min,max): (" << minval << "," << maxval << ")" << std::endl;
    	return pix;
    }

    unsigned char *touchar(float *pix, const int &size)
    {
    	unsigned char *res = new unsigned char [size];
    	for(int i = 0;i < size;i++)
    	{
    		res[i] = touchar(pix[i]);
    	}
    	return res;
    }

    void toPPM(const std::string &filename, unsigned char *pix, const int &width, const int &height)
    {
    	std::ofstream f(filename);
    	f << "P3\n" << width << " " << height << "\n255\n";
    	for(int i = 0;i < width * height;i++)
    	{
    		f << (int) pix[i];
    	}
    }
};