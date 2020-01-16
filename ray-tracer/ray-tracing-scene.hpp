#ifndef __RAY_TRACING_SCENE_HPP__
#define __RAY_TRACING_SCENE_HPP__

#include "vec3.hpp"
#include "shape.hpp"
#include "mat4.hpp"
#include "basic-math.hpp"
#include "transform.hpp"

#include <vector>
#include <fstream>
#include <algorithm>

namespace rt
{
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

        static RayTracingScene FromScene(const std::string &filename, const std::string &data_dir);
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
    };

}; // namespace

#endif 