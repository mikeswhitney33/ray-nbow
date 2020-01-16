#ifndef __RAY_TRACER_HPP__
#define __RAY_TRACER_HPP__

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

/**
 * vec3:
 * -----
 * The basic 3d vector structure
 */
typedef struct vec3
{
    float x, y, z;
} vec3;

/**
 * mat4:
 * -----
 * A basic structure representing a 4x4 matrix.
 */
typedef struct mat4
{
    float m[4][4];
} mat4;

class Transform
{
public:
    Transform(const vec3 &t={0, 0, 0}, const vec3 &r={0, 0, 0}, const vec3 &s={1, 1, 1});
    mat4 mat() const;
private:
    vec3 translation, rotation, scaling;
};

std::ostream &operator<<(std::ostream &os, const mat4 &m);
std::istream &operator>>(std::istream &is, vec3 &v);

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

float radians(const float &deg);


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

    /**
     * orig:
     * -----
     * an accessor method for getting the origin.
     */
    vec3 orig() const;

    /**
     * dir:
     * ----
     * an accessor method for getting the direction.
     */
    vec3 dir() const;
private:
    vec3 o, d;
};

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
    float radius, radius2;
};

/**
 * RayTracingScene:
 * ----------------
 * a class representing a ray tracing scene.
 */
class RayTracingScene
{
public:
    // Default values
    static constexpr int DEFAULT_WIDTH = 800;
    static constexpr int DEFAULT_HEIGHT = 600;
    static constexpr float DEFAULT_FOV = 60.0f;
    static constexpr vec3 DEFAULT_EYE = {0, 0, -1};
    static constexpr vec3 DEFAULT_CENTER = {0, 0, 0};
    static constexpr vec3 DEFAULT_UP = {0, 1, 0};
    /**
     * RayTracingScene:
     * ----------------
     * constructs a scene using default values.
     */
    RayTracingScene();

    /**
     * RayTracingScene:
     * ----------------
     * constructs a scene with user provided values.
     *
     * @param width: int the width of the scene.
     * @param height: int the height of the scene.
     * @param fov: float the field of view for the scene.
     */
    RayTracingScene(const int &width, const int &height, const float &fov);

    /**
     * getDistances:
     * -------------
     * retrieve the distances from a camera to the scene geometry.
     *
     * @param eye: vec3 the position of the camera.
     * @param center: vec3 the position the camera is looking at.
     * @param up: vec3 a general up vector for the camera.
     * @return the distances from camera to scene geometry.
     * NOTE: the values from this function are dynamically allocated and need to be cleaned up by the caller.
     */
    float *getDistances(const vec3 &eye=DEFAULT_EYE, const vec3 &center=DEFAULT_CENTER, const vec3 &up=DEFAULT_UP) const;

    /**
     * getDistances:
     * -------------
     * retrieve the distances from a camera to the scene geometry.
     *
     * @param camera: mat4 the camera matrix
     * @return the distances from camera to scene geometry.
     * NOTE: the values from this function are dynamically allocated and need to be cleaned up by the caller.
     */
    float *getDistances(const mat4 &camera) const;

    /**
     * savePPM:
     * --------
     * saves the distances to a ppm file for visualization.
     *
     * @param filename: string the name of the output file.
     * @param pix: float* the distances to save
     */
    void savePPM(const std::string &filename, float *pix) const;

    /**
     * addShape:
     * ---------
     * adds a shape to the scene.
     *
     * @param s: Shape* the shape to add
     */
    void addShape(Shape *s);

    /**
     * addObj:
     * -------
     * add triangles from an obj file.
     *
     * @param filename: string the name of the obj file.
     */
    void addObj(const std::string &filename, const Transform &t=Transform());

    void setWidth(const int &width);
    void setHeight(const int &height);
    void setFov(const float &fov);

    static RayTracingScene FromScene(const std::string &filename);
private:
    int width, height;
    float w, h, fov, scale, aspect;
    std::vector<Shape *> shapes;

    /**
     * traceDistance:
     * --------------
     * traces a ray to get the distance to the intersection.
     *
     * @param ray : Ray the ray we are tracing.
     * @returns the distace to the intersection or -1 if no intersection occured.
     */
    float traceDistance(const Ray &ray) const;

    /**
     * touchar:
     * --------
     * converts a float to an unsigned char for saving to images.
     *
     * @param f: float the input float
     * @return f but as a unsigned char ranged between 0 and 255
     */
    unsigned char touchar(const float &f) const;
};


#endif // __RAY_TRACER_HPP__
