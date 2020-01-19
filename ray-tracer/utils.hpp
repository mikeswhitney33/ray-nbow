#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>

namespace rt
{
    /**
     * radians:
     * --------
     * converts degress to radians
     *
     * @param deg: float the input degrees.
     * @return deg but in radians.
     */
    float radians(const float &deg);

    /**
     * touchar:
     * --------
     * converts a float to an unsigned char for saving to images.
     *
     * @param f: float the input float
     * @return f but as a unsigned char ranged between 0 and 255
     */
    unsigned char touchar(const float &f);

    unsigned char *touchar(float *pix, const int &size);

    float *normalize(float *pix, const int &size, bool invert=false);

    void toPPM(const std::string &filename, unsigned char *pix, const int &size);
};



#endif // __BASIC_MATH_HPP__