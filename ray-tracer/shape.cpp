#include "shape.hpp"

namespace rt
{
    Triangle::Triangle(const vec3 &a, const vec3 &b, const vec3 &c):a(a),b(b),c(c)
    {}

    bool Triangle::intersect(const Ray &ray, float &t) const
    {
        vec3 ab = b - a;
        vec3 ac = c - a;
        vec3 pvec = cross(ray.dir(), ac);
        float det = dot(ab, pvec);
        if(fabs(det) < std::numeric_limits<float>::epsilon()) return false;
        float idet = 1 / det;
        vec3 tvec = ray.orig() - a;
        float u = dot(tvec, pvec) * idet;
        if(u < 0 || u > 1) return false;
        vec3 qvec = cross(tvec, ab);
        float v = dot(ray.dir(), qvec) * idet;
        if(v < 0 || u + v > 1) return false;
        float t0 = dot(ac, qvec) * idet;
        if(t0 > t) return false;
        t = t0;
        return true;
    }

    Sphere::Sphere(const vec3 &c, const float &r):center(c),radius2(r*r)
    {}

    bool Sphere::intersect(const Ray &ray, float &t) const 
    {
        vec3 L = center - ray.orig();
        float tca = dot(L, ray.dir());
        if(tca < 0) return false;  // ray is facing the wrong way
        float d2 = dot(L, L) - tca * tca;
        if(d2 > radius2) return false;  // ray misses the sphere
        float thc = sqrt(radius2 - d2);
        float t0 = tca - thc;
        float t1 = tca + thc;
        if(t0 > t1) std::swap(t0, t1); 
        if(t0 < 0) t0 = t1; // we are inside the sphere
        if(t0 < 0 || t0 > t) return false; // the sphere is behind us or another shape is infront of it.
        t = t0;
        return true;
    }
};