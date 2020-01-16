#include "basic-math.hpp"

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
};