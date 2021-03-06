#ifndef __RAY_HPP__
#define __RAY_HPP__

#include "vec3.hpp"

namespace rt
{
    /**
     * Ray:
     * ----
     * a representation of a ray
     */
    class Ray
    {
    public:
        /**
         * Ray:
         * ----
         * constructs a new Ray.
         *
         * @param o: vec3 the origin of the ray.
         * @param d: vec3 the direction of the ray.
         */
        Ray(const vec3 &o, const vec3 &d);

        vec3 orig, dir, invdir;
        int sign[3];
    };

}; // namespace

#endif 