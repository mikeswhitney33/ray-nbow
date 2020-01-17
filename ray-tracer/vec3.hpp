#ifndef __VEC3_HPP__
#define __VEC3_HPP__

#include <iostream>
#include <cmath>

namespace rt 
{
    /**
     * vec3:
     * -----
     * The basic 3d vector structure
     */
    typedef struct vec3
    {
        float x, y, z;
    } vec3;

    std::istream &operator>>(std::istream &is, vec3 &v);
    /**
     * operator+:
     * ----------
     * performs addition between two vec3's
     *
     * @param a: vec3 the left operand
     * @param b: vec3 the right operand
     * @return the elementwise addition of two vec3's
     */
    vec3 operator+(const vec3 &a, const vec3 &b);

    /**
     * operator-:
     * ----------
     * performs subtraction between two vec3's
     *
     * @param a: vec3 the left operand
     * @param b: vec3 the right operand
     * @return the elementwise subtraction of two vec3's
     */
    vec3 operator-(const vec3 &a, const vec3 &b);

    /**
     * operator*:
     * ----------
     * performs multiplication between a vec3 and a float
     *
     * @param a: vec3 the left operand
     * @param f: float the right operand
     * @return the elementwise multiplication between a vec3 and a float
     */
    vec3 operator*(const vec3 &a, const float &f);

    /**
     * operator/:
     * ----------
     * performs division between a vec3 and a float
     *
     * @param a: vec3 the left operand
     * @param f: float the right operand
     * @return the elementwise division between a vec3 and a float
     */
    vec3 operator/(const vec3 &a, const float &f);
    vec3 operator/(const float &f, const vec3 &a);

    /**
     * dot:
     * ----
     * performs the dot product between two vec3's
     *
     * @param a: vec3 the left operand
     * @param b: vec3 the right operand
     * @return the dot product between a and b.
     */
    float dot(const vec3 &a, const vec3 &b);

    /**
     * cross:
     * ----
     * performs the cross product between two vec3's
     *
     * @param a: vec3 the left operand
     * @param b: vec3 the right operand
     * @return the cross product between a and b.
     */
    vec3 cross(const vec3 &a, const vec3 &b);

    /*
     * norm:
     * -----
     * normalizes a given vec3
     *
     * @param a: vec3 the given vec3
     * @return: the normalized version of a
     */
    vec3 norm(const vec3 &a);

    float min(const vec3 &v);
    float max(const vec3 &v);

}; // namespace

#endif