#include "ray-tracing-scene.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

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

    RayTracingScene::RayTracingScene(const int &width, const int &height, const float &fov):width(width), height(height),w(width), h(height), fov(fov), scale(tanf(fov * M_PI / 180.0f * 0.5f)), aspect(w/h)
    {}

    float *RayTracingScene::getDistances(const vec3 &eye, const vec3 &center, const vec3 &up) const
    {
        mat4 camera = lookAt(eye, center, up);
        return getDistances(camera);
    }

    float *RayTracingScene::getDistances(const mat4 &camera) const
    {
        float *pix = new float[width * height];
        int k = 0;
        vec3 orig = transformPt(camera, {0, 0, 0});
        for(int j = 0;j < height;j++)
        {
            for(int i = 0;i < width;i++)
            {
                float x = (2.0f * (i + 0.5f) / w - 1.0f) * scale * aspect;
                float y = (1.0f - 2.0f * (j + 0.5f) / h) * scale;
                vec3 dir = transformDir(camera, {x, y, 1});
                float c = traceDistance(Ray(orig, norm(dir)));
                pix[k++] = c;
            }
        }
        return pix;
    }

    void RayTracingScene::savePPM(const std::string &filename, float *pix) const
    {
        int size = width * height;
        float maxval = 0;
        float minval = std::numeric_limits<float>::max();
        for(int i = 0;i < size;i++)
        {
            if(pix[i] > maxval)
            {
                maxval = pix[i];
            }
            if(pix[i] >= 0 && pix[i] < minval)
            {
                minval = pix[i];
            }
        }
        for(int i = 0;i < size;i++)
        {
            pix[i] = (pix[i] - minval) / (maxval - minval);
        }
        std::ofstream f(filename);
        f << "P3\n" << width << " " << height << "\n255\n";
        for(int i = 0;i < size;i++)
        {
            f << (int)touchar(pix[i]) << " " << (int)touchar(pix[i]) << " " << (int)touchar(pix[i]) << " ";
        }
    }

    void RayTracingScene::addShape(Shape *s)
    {
        shapes.push_back(s);
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
                shapes.push_back(new Triangle(verts[0], verts[1], verts[2]));
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

    RayTracingScene RayTracingScene::FromScene(const std::string &filename, const std::string &data_dir)
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
                scene.addObj(data_dir + path, Transform(t, r, s));
            }
        }
        return scene;
    }

    float RayTracingScene::traceDistance(const Ray &ray) const
    {
        float t = std::numeric_limits<float>::max();
        bool hit = false;
        for(auto s : shapes)
        {
            if(s->intersect(ray, t))
            {
                hit = true;
            }
        }
        return hit ? t : -1;
    }

}; // namespace
