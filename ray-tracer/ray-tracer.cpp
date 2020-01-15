#include "ray-tracer.hpp"

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

Sphere::Sphere(const vec3 &c, const float &r):center(c),radius(r),radius2(r*r)
{}
bool Sphere::intersect(const Ray &ray, float &t) const 
{
    vec3 L = center - ray.orig();
    float tca = dot(L, ray.dir());
    if(tca < 0) return false;
    float d2 = dot(L, L) - tca * tca;
    if(d2 > radius2) return false;
    float thc = sqrt(radius2 - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;
    if(t0 > t1) std::swap(t0, t1);
    if(t0 < 0) t0 = t1;
    if(t0 < 0 || t0 > t) return false;
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
    float *pix = new float[width * height];
    int k = 0;
    for(int j = 0;j < height;j++)
    {
        for(int i = 0;i < width;i++)
        {
            float x = (2.0f * (i + 0.5f) / w - 1.0f) * scale * aspect;
            float y = (1.0f - 2.0f * (j + 0.5f) / h) * scale;
            float c = traceDistance(Ray({0, 0, -1}, norm({x, y, 1})));
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
            std::cout << "Hit: " << t << std::endl;
        }
    }
    return hit ? t : -1;
}

unsigned char RayTracingScene::touchar(const float &f) const
{
    return (f < 0 ? 0 : f > 1 ? 1 : f) * 255.0f;
}

void RayTracingScene::savePPM(const std::string &filename, const vec3 &eye, const vec3 &center, const vec3 &up) const
{
    float *pix = getDistances(eye, center, up);
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
    std::cout << "Max: " << maxval << ", " << "Min: " << minval << std::endl;
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
