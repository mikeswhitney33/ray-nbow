#ifndef __SHAPE_HPP__
#define __SHAPE_HPP__

#include "vec3.hpp"
#include "ray.hpp"

#include <cmath>
#include <vector>

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
        virtual void extents(vec3 &emin, vec3 &emax) const = 0;
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
        virtual void extents(vec3 &emin, vec3 &emax) const;
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
        virtual void extents(vec3 &emin, vec3 &emax) const;
    private:
        vec3 center;
        float radius, radius2;
    };

    bool raybox(const Ray &ray, const vec3 bounds[2]);
    bool boxbox(const vec3 bounds1[2], const vec3 bounds2[2]);

    class BoundingBox: public Shape
    {
    public:
        BoundingBox(Shape *s);
        virtual bool intersect(const Ray &ray, float &t) const;
        virtual void extents(vec3 &emin, vec3 &emax) const;
    private:
        vec3 bounds[2];
        Shape *shape;
    };

    class ShapeContainer
    {
    public:
        virtual bool intersect(const Ray &ray, float &t) const = 0;
        virtual void addShape(Shape * shape) = 0;
        virtual size_t size() const = 0;
    };

    class LinearContainer: public ShapeContainer
    {
    public:
        virtual bool intersect(const Ray &ray, float &t) const;
        virtual void addShape(Shape *shape);
        virtual size_t size() const;
    private:
        std::vector<Shape*>shapes;
    };

    class MassBoxContainer: public ShapeContainer
    {
    public:
        virtual bool intersect(const Ray &ray, float &t) const;
        virtual void addShape(Shape *shape);
        virtual size_t size() const;
    private:
        vec3 bounds[2];
        LinearContainer shapes;
    };


    struct OctreeNode
    {
        static constexpr size_t MAX_SIZE = 5;
        static constexpr size_t MAX_DEPTH = 8;
        vec3 center;
        std::vector<Shape*> content;
        std::vector<OctreeNode> children;
        vec3 bounds[2];
        size_t depth;
        OctreeNode(const vec3 &emin, const vec3 &emax, size_t depth);
        void split();
        void addShape(Shape *shape);
        bool intersect(const Ray &ray, float &t) const;
        bool intersect(Shape *shape) const;
    };

}; // namespace


#endif