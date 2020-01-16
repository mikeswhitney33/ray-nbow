#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__

#include "vec3.hpp"
#include "mat4.hpp"

namespace rt
{
    class Transform
    {
    public:
        Transform(const vec3 &t={0, 0, 0}, const vec3 &r={0, 0, 0}, const vec3 &s={1, 1, 1});
        mat4 mat() const;
    private:
        vec3 translation, rotation, scaling;
    };
};
    

#endif // __TRANSFORM_HPP__