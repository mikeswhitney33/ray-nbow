#include "ray-tracer.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

std::ostream &operator<<(std::ostream &os, const mat4 &m)
{
    for(int i = 0;i < 4;i++)
    {
        if(i > 0) os << "\n";
        for(int j = 0;j < 4;j++)
        {
            if(j > 0) os << ",";
            os << m.m[i][j];
        }
    }
    return os;
}

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

float radians(const float &deg)
{
    return deg * M_PI / 180.0f;
}

mat4 Transform::mat() const
{
    mat4 t = translate(translation);
    mat4 s = scale(scaling);
    mat4 rx = rotate(rotation.x, {1, 0, 0});
    mat4 ry = rotate(rotation.y, {0, 1, 0});
    mat4 rz = rotate(rotation.z, {0, 0, 1});
    return matmul(t, matmul(rx, matmul(ry, matmul(rz, s))));
}
Transform::Transform(const vec3 &t, const vec3 &r, const vec3 &s):translation(t),scaling(s), rotation(r)
{}

mat4 identity()
{
    return mat4{
        {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        }
    };
}

mat4 translate(const float &dx, const float &dy, const float &dz)
{
    return mat4{
        {
            {1, 0, 0, dx},
            {0, 1, 0, dy},
            {0, 0, 1, dz},
            {0, 0, 0, 1}
        }
    };
}
mat4 translate(const vec3 &d)
{
    return translate(d.x, d.y, d.z);
}
mat4 scale(const float &dx, const float &dy, const float &dz)
{
    return mat4{
        {
            {dx, 0, 0, 0},
            {0, dy, 0, 0},
            {0, 0, dz, 0},
            {0, 0, 0, 1}
        }
    };
}
mat4 scale(const vec3 &d)
{
    return scale(d.x, d.y, d.z);
}
mat4 scale(const float &s)
{
    return scale(s, s, s);
}

mat4 lookAt(const vec3 &eye, const vec3 &center, const vec3 &up)
{
    vec3 f = norm(eye - center);
    vec3 s = norm(cross(f, up));
    vec3 u = norm(cross(s, f));
    return mat4{
        {
            {s.x, s.y, s.z, -eye.x},
            {u.x, u.y, u.z, -eye.y},
            {f.x, f.y, f.z, -eye.z},
            {0, 0, 0, 1}
        }
    };
}

vec3 transformPt(const mat4 &m, const vec3 &v)
{
    return vec3{
        m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3],
        m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3],
        m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3]
    };
}
vec3 transformDir(const mat4 &m, const vec3 &v)
{
    return vec3{
        m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z,
        m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z,
        m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z
    };
}

mat4 rotate(const float &angle, const vec3 &axis)
{
    float c = cos(angle);
    float s = sin(angle);
    vec3 ax = norm(axis);
    vec3 tmp = ax * (1 - c);

    return mat4{
        {
            {c + tmp.x * ax.x, tmp.x * ax.y + s * ax.z, tmp.x * ax.z - s * ax.y, 0},
            {tmp.y * ax.x - s * ax.z, c + tmp.y * ax.y, tmp.y * ax.z + s * ax.x, 0},
            {tmp.z * ax.x + s * ax.y, tmp.z * ax.y - s * ax.x, c + tmp.z * ax.z, 0},
            {0, 0, 0, 1}
        }
    };
}

mat4 matmul(const mat4 &m1, const mat4 &m2)
{
    mat4 res;
    for(int i =0;i < 4;i++)
    {
        for(int j = 0;j < 4;j++)
        {
            res.m[i][j] = 0;
            for(int k = 0;k < 4;k++)
            {
                res.m[i][j] += m1.m[i][k] * m2.m[j][k];
            }
        }
    }
    return res;
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

Ray::Ray(const vec3 &o, const vec3 &d):o(o), d(d)
{}

vec3 Ray::orig() const 
{
    return o;
}

vec3 Ray::dir() const
{
    return d;
}

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

Sphere::Sphere(const vec3 &c, const float &r):center(c),radius(r),radius2(r*r)
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

unsigned char RayTracingScene::touchar(const float &f) const
{
    return (f < 0 ? 0 : f > 1 ? 1 : f) * 255.0f;
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
            int fv = objshapes[s].mesh.num_face_vertices[f];
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
    }
    return scene;

}
