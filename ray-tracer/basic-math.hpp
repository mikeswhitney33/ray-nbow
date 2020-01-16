#ifndef __BASIC_MATH_HPP__
#define __BASIC_MATH_HPP__

#include <cmath>

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
};



#endif // __BASIC_MATH_HPP__