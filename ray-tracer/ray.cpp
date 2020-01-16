#include "ray.hpp"

namespace rt
{
    Ray::Ray(const vec3 &o, const vec3 &d):o(o),d(d)
    {}
    /**
     * orig:
     * -----
     * an accessor method for getting the origin.
     */
    vec3 Ray::orig() const
    {
        return o;
    }

    /**
     * dir:
     * ----
     * an accessor method for getting the direction.
     */
    vec3 Ray::dir() const
    {
        return d;
    }
};