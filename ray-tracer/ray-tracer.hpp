#ifndef __RAY_TRACER_HPP__
#define __RAY_TRACER_HPP__

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>

struct vec3 
{
    float x, y, z;
};

vec3 operator+(const vec3 &a, const vec3 &b);
vec3 operator-(const vec3 &a, const vec3 &b);
vec3 operator*(const vec3 &a, const float &f);
vec3 operator/(const vec3 &a, const float &f);
float dot(const vec3 &a, const vec3 &b);
vec3 norm(const vec3 &a);

class Ray
{
public:
    Ray(const vec3 &o, const vec3 &d);
    vec3 orig() const;
    vec3 dir() const;
private:
    vec3 o, d;
};

class Shape
{
public:
    virtual bool intersect(const Ray &ray, float &t) const = 0;
};

class Sphere: public Shape
{
public:
    Sphere(const vec3 &c, const float &r);
    virtual bool intersect(const Ray &ray, float &t) const;
private:
    vec3 center;
    float radius, radius2;
};


class RayTracingScene
{
public:
    static constexpr int DEFAULT_WIDTH = 800;
    static constexpr int DEFAULT_HEIGHT = 600;
    static constexpr float DEFAULT_FOV = 60.0f;
    static constexpr vec3 DEFAULT_EYE = {0, 0, -1};
    static constexpr vec3 DEFAULT_CENTER = {0, 0, 0};
    static constexpr vec3 DEFAULT_UP = {0, 1, 0};
    RayTracingScene();
    RayTracingScene(const int &width, const int &height, const float &fov);
    float *getDistances(const vec3 &eye=DEFAULT_EYE, const vec3 &center=DEFAULT_CENTER, const vec3 &up=DEFAULT_UP) const;
    void savePPM(const std::string &filename, const vec3 &eye=DEFAULT_EYE, const vec3 &center=DEFAULT_CENTER, const vec3 &up=DEFAULT_UP) const;
    void addShape(Shape *);
private:
    int width, height;
    float w, h, fov, scale, aspect;
    std::vector<Shape *> shapes;

    float traceDistance(const Ray &ray) const;
    unsigned char touchar(const float &f) const;
};


#endif // __RAY_TRACER_HPP__
