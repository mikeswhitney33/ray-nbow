#ifndef __SHAPE_HPP__
#define __SHAPE_HPP__

#include "vec3.hpp"
#include "ray.hpp"

#include <cmath>

namespace rt
{
    /**
     * Shape:
     * ------
     * an interface representing any given shape.
     */
    class Shape
    {
    public:
        /**
         * intersect:
         * ----------
         * performs an intersection test between the shape and a ray.
         *
         * @param ray: Ray the incoming ray.
         * @param t: float the distance from the ray origin to the intersection point.
         * @return true if hit, false otherwise.
         */
        virtual bool intersect(const Ray &ray, float &t) const = 0;
    };

    /**
     * Triangle:
     * ---------
     * an implementation of the shape class to represent a triangle.
     */
    class Triangle: public Shape
    {
    public:
        /**
         * Triangle:
         * ---------
         * constructs a new triangle given the vertices.
         *
         * @param a: vec3 the first vertex.
         * @param b: vec3 the second vertex.
         * @param c: vec3 the third vertex.
         */
        Triangle(const vec3 &a, const vec3 &b, const vec3 &c);

        /**
         * intersect:
         * ----------
         * performs an intersection test between the shape and a ray.
         *
         * @param ray: Ray the incoming ray.
         * @param t: float the distance from the ray origin to the intersection point.
         * @return true if hit, false otherwise.
         */
        virtual bool intersect(const Ray &ray, float &t) const;
    private:
        vec3 a, b, c;
    };

    /**
     * Sphere:
     * -------
     * an implementation of the shape class to represent a sphere.
     */
    class Sphere: public Shape
    {
    public:
        /**
         * Sphere:
         * -------
         * constructs a new sphere given a center and radius.
         *
         * @param c: vec3 the center of the sphere.
         * @param r: float the radius of the sphere.
         */
        Sphere(const vec3 &c, const float &r);

        /**
         * intersect:
         * ----------
         * performs an intersection test between the shape and a ray.
         *
         * @param ray: Ray the incoming ray.
         * @param t: float the distance from the ray origin to the intersection point.
         * @return true if hit, false otherwise.
         */
        virtual bool intersect(const Ray &ray, float &t) const;
    private:
        vec3 center;
        float radius2;
    };

}; // namespace


#endif