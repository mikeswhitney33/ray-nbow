#include "vec3.hpp"

namespace rt
{
    std::istream &operator>>(std::istream &is, vec3 &v)
    {
        is.ignore(256, '(');
        is >> v.x;
        is.ignore(256, ',');
        is >> v.y;
        is.ignore(256, ',');
        is >> v.z;
        is.ignore(256, ')');
        return is;
    }
    vec3 operator+(const vec3 &a, const vec3 &b)
    {
        return {a.x + b.x, a.y + b.y, a.z + b.z};
    }
    vec3 operator-(const vec3 &a, const vec3 &b)
    {
        return {a.x - b.x, a.y - b.y, a.z - b.z};
    }
    vec3 operator*(const vec3 &a, const float &f)
    {
        return {a.x * f, a.y * f, a.z * f};
    }
    vec3 operator/(const vec3 &a, const float &f)
    {
        return {a.x / f, a.y / f, a.z / f};
    }
    float dot(const vec3 &a, const vec3 &b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    vec3 cross(const vec3 &a, const vec3 &b)
    {
        return 
        {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }
    vec3 norm(const vec3 &a)
    {
        return a / sqrtf(dot(a, a));
    }
};