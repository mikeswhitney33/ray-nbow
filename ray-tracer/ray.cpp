#include "ray.hpp"

namespace rt
{
    Ray::Ray(const vec3 &o, const vec3 &d):orig(o),dir(d),invdir(1/d)
    {
        sign[0] = d.x < 0;
        sign[1] = d.y < 0;
        sign[2] = d.z < 0;
    }
};