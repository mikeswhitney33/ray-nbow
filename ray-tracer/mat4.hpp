#ifndef __MAT4_HPP__
#define __MAT4_HPP__

#include <iostream>
#include <cmath>

#include "vec3.hpp"

namespace rt
{
      /**
     * mat4:
     * -----
     * A basic structure representing a 4x4 matrix.
     */
    typedef struct mat4
    {
        float m[4][4];
    } mat4;

    std::ostream &operator<<(std::ostream &os, const mat4 &m);

    mat4 identity();
    mat4 translate(const float &dx, const float &dy, const float &dz);
    mat4 translate(const vec3 &d);
    mat4 scale(const float &dx, const float &dy, const float &dz);
    mat4 scale(const vec3 &d);
    mat4 scale(const float &s);
    mat4 rotate(const float &angle, const vec3 &axis);
    mat4 lookAt(const vec3 &eye, const vec3 &center, const vec3 &up);

    vec3 transformPt(const mat4 &m, const vec3 &v);
    vec3 transformDir(const mat4 &m, const vec3 &v);
    mat4 matmul(const mat4 &m1, const mat4 &m2);  
};



#endif 