#include "ray-tracing-scene.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../include/tiny_obj_loader.h"

#include <queue>
#include <thread>

constexpr float MAX_FLOAT = std::numeric_limits<float>::max();

namespace rt
{

    constexpr int RayTracingScene::DEFAULT_WIDTH;
    constexpr int RayTracingScene::DEFAULT_HEIGHT;
    constexpr float RayTracingScene::DEFAULT_FOV;
    constexpr vec3 RayTracingScene::DEFAULT_EYE;
    constexpr vec3 RayTracingScene::DEFAULT_CENTER;
    constexpr vec3 RayTracingScene::DEFAULT_UP;

    RayTracingScene::RayTracingScene():RayTracingScene(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_FOV)
    {}

    RayTracingScene::RayTracingScene(const int &width, const int &height, const float &fov):width(width), height(height),w(width), h(height), fov(fov), scale(tanf(fov * M_PI / 180.0f * 0.5f)), aspect(w/h), eye(DEFAULT_EYE), center(DEFAULT_CENTER), up(DEFAULT_UP), verbosity(false)
    {
        shapes = new MassBoxContainer();
    }


    void getDistSingle(const RayTracingScene * self, float *pix, const int &k, const Ray &ray)
    {
        pix[k] = self->traceDistance(ray);
    }

    float *RayTracingScene::getDistances(std::function<void(int, int)> callback) const
    {
        mat4 camera = lookAt(eye, center, up);
        int total = width * height;
        float *pix = new float[total];
        int k = 0;
        vec3 orig = transformPt(camera, {0, 0, 0});

        for(int j = 0;j < height;j++)
        {
            if(verbosity) std::cout << j << "/" << height << std::endl;
            for(int i = 0;i < width;i++)
            {
                float x = (2.0f * (i + 0.5f) / w - 1.0f) * scale * aspect;
                float y = (1.0f - 2.0f * (j + 0.5f) / h) * scale;
                vec3 dir = transformDir(camera, {x, y, 1});
                Ray ray(orig, norm(dir));
                getDistSingle(this, pix, k, ray);
                k++;
            }
            if(verbosity)
                callback(j, height);
        }
        return pix;
    }

    void RayTracingScene::addShape(Shape *s)
    {
        shapes->addShape(new BoundingBox(s));
    }

    void RayTracingScene::addObj(const std::string &filename, const Transform &t)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> objshapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;
        bool ret = tinyobj::LoadObj(&attrib, &objshapes, &materials, &warn, &err, filename.c_str());
        if(!warn.empty())
        {
            std::cout << warn << std::endl;
        }

        if(!err.empty())
        {
            std::cerr << err << std::endl;
        }

        if(!ret)
        {
            return;
        }

        mat4 m = t.mat();
        for(size_t s = 0;s < objshapes.size();s++)
        {
            size_t index_offset = 0;
            for(size_t f = 0;f < objshapes[s].mesh.num_face_vertices.size();f++)
            {
                size_t fv = objshapes[s].mesh.num_face_vertices[f];
                vec3 verts[3];
                for(size_t v = 0;v < fv;v++)
                {
                    tinyobj::index_t idx = objshapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t x = attrib.vertices[3 * idx.vertex_index + 0];
                    tinyobj::real_t y = attrib.vertices[3 * idx.vertex_index + 1];
                    tinyobj::real_t z = attrib.vertices[3 * idx.vertex_index + 2];
                    verts[v] = transformPt(m, {x, y, z});
                }
                addShape(new BoundingBox(new Triangle(verts[0], verts[1], verts[2])));
                index_offset += fv;
            }
        }
    }

    void RayTracingScene::setWidth(const int &width)
    {
        this->width = width;
        w = width;
        aspect = w / h;
    }
    void RayTracingScene::setHeight(const int &height)
    {
        this->height = height;
        h = height;
        aspect = w / h;
    }
    void RayTracingScene::setFov(const float &fov)
    {
        this->fov = fov;
        scale = tanf(radians(fov));
    }

    int RayTracingScene::getWidth() const
    {
        return width;
    }
    int RayTracingScene::getHeight() const
    {
        return height;
    }
    int RayTracingScene::getDims() const
    {
        return width * height;
    }

    size_t RayTracingScene::size() const
    {
        return shapes->size();
    }

    void RayTracingScene::setVerbosity(const bool &v)
    {
        verbosity = v;
    }

    void RayTracingScene::setEye(const vec3 &v)
    {
        eye = v;
    }

    void RayTracingScene::setCenter(const vec3 &v)
    {
        center = v;
    }

    void RayTracingScene::setUp(const vec3 &v)
    {
        up = v;
    }

    RayTracingScene RayTracingScene::FromScene(const std::string &filename)
    {
        RayTracingScene scene;
        std::ifstream f(filename);
        if(!f.is_open())
        {
            std::cerr << "Error reading: " << filename << ".  Using default settings." << std::endl;
            perror(filename.c_str());
            return scene;
        }
        std::string label;
        while(f >> label)
        {
            std::transform(label.begin(), label.end(), label.begin(), [](unsigned char c){return std::tolower(c);});
            if(label == "width")
            {
                int width;
                f >> width;
                scene.setWidth(width);
            }
            else if(label == "height")
            {
                int height;
                f >> height;
                scene.setHeight(height);
            }
            else if(label == "fov")
            {
                float fov;
                f >> fov;
                scene.setFov(fov);
            }
            else if(label == "sphere")
            {
                vec3 center;
                float r;
                f >> center >> r;
                scene.addShape(new Sphere(center, r));
            }
            else if(label == "triangle")
            {
                vec3 a,b,c;
                f >> a >> b >> c;
                scene.addShape(new Triangle(a, b, c));
            }
            else if(label == "obj")
            {
                std::string path;
                vec3 t, r, s;
                f >> path >> t >> r >> s;
                scene.addObj(path, Transform(t, r, s));
            }
            else if(label == "eye")
            {
                vec3 eye;
                f >> eye;
                scene.setEye(eye);
            }
            else if(label == "center")
            {
                vec3 center;
                f >> center;
                scene.setCenter(center);
            }
            else if(label == "up")
            {
                vec3 up;
                f >> up;
                scene.setUp(up);
            }
            else if(label == "camera")
            {
                vec3 eye, center, up;
                f >> eye >> center >> up;
                scene.setEye(eye);
                scene.setCenter(center);
                scene.setUp(up);
            }
        }

        std::cout << filename << std::endl;
        std::cout << "[w x h]: " << "[" << scene.width << " x " << scene.height << "]" << std::endl;
        std::cout << "fov: " << scene.fov << std::endl;
        std::cout << "Number of shapes: " << scene.size() << std::endl;
        return scene;
    }

    float RayTracingScene::traceDistance(const Ray &ray) const
    {
        float t = std::numeric_limits<float>::max();
        return shapes->intersect(ray, t) ? t : 0;
    }

}; // namespace
