#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <random>
#include <functional>
#include <chrono>
#include <thread>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../include/tiny_obj_loader.h"

std::default_random_engine gen;


union vec3 {
    struct {
        float x, y, z;
    };
    float data[3];
    vec3(float x=0,float y=0,float z=0):x(x),y(y),z(z){};
};

struct SceneInfo {
    int width, height;
    float fov;
    vec3 ambient;
    vec3 background;
    size_t samples;
};

struct mat4 {
    float data[4][4];
};

bool intersect(int &id, float &t);
vec3 trace(const vec3 &ro, const vec3 &rd, int depth);
std::string get_resources();
bool raybox(const vec3 &ro, const vec3 &rd, vec3 bounds[2], float &t);

std::istream &operator>>(std::istream &is, vec3 &v)
{
    is.ignore(256, '(');
    is >> v.x;
    is.ignore(256,',');
    is >> v.y;
    is.ignore(256,',');
    is >> v.z;
    is.ignore(256,')');
    return is;
}

std::ostream &operator<<(std::ostream &os, const vec3 &v)
{
    return os << "(" << v.x << "," << v.y << "," << v.z << ")";
}

vec3 operator+(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.x+v2.x,v1.y+v2.y,v1.z+v2.z);
}

vec3 operator+(const vec3 &v, const float &f)
{
    return vec3(v.x+f, v.y+f, v.z+f);
}

vec3 operator-(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.x-v2.x,v1.y-v2.y,v1.z-v2.z);
}

vec3 operator-(const vec3 &v)
{
    return vec3(-v.x,-v.y,-v.z);
}

vec3 operator-(const vec3 &v, const float &f)
{
    return vec3(v.x-f,v.y-f,v.z-f);
}

vec3 operator/(const vec3 &v, const float &f)
{
    return vec3(v.x/f,v.y/f,v.z/f);
}

vec3 operator/(const float &f, const vec3 &v)
{
    return vec3(f/v.x, f/v.y,f/v.z);
}

vec3 operator*(const vec3 &v, const float &f)
{
    return vec3(v.x*f,v.y*f,v.z*f);
}

vec3 operator*(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.x*v2.x,v1.y*v2.y,v1.z*v2.z);
}

vec3 cross(const vec3 &v1, const vec3 &v2)
{
    return vec3(
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    );
}

float min(const vec3 &v)
{
    return v.x < v.y && v.x < v.z ? v.x : v.y < v.z ? v.y : v.z;
}

float max(const vec3 &v)
{
    return v.x > v.y && v.x > v.z ? v.x : v.y > v.z ? v.y : v.z;
}

float sum(const vec3 &v)
{
    return v.x + v.y + v.z;
}

float dot(const vec3 &v1, const vec3 &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float mag(const vec3 &v)
{
    return dot(v,v);
}

float len(const vec3 &v)
{
    return sqrt(mag(v));
}

vec3 norm(const vec3 &v)
{
    return v / len(v);
}

float clip(const float &f)
{
    return f < 0 ? 0 : f > 1 ? 1 : f;
}

float clamp(const float &src, const float &min, const float &max)
{
    return src < min ? min : src > max ? max : src;
}

vec3 clip(const vec3 &v)
{
    return vec3(clip(v.x), clip(v.y), clip(v.z));
}

int scaleValue(const float &f) 
{
    return 255 * clip(f);
}

float radians(const float &deg)
{
    return deg * M_PI / 180.0f;
}

mat4 matmul(const mat4 &m1, const mat4 &m2)
{
    mat4 res;
    for(int i = 0;i < 4;i++)
    {
        for(int j = 0;j < 4;j++)
        {
            res.data[i][j] = 0;
            for(int k = 0;k < 4;k++)
            {
                res.data[i][j] += m1.data[i][k] * m2.data[k][j];
            }
        }
    }
    return res;
}

vec3 transform(const mat4 &m, float v[4])
{
    float res[4];
    for(int i = 0;i < 4;i++)
    {
        res[i] = 0;
        for(int j = 0;j < 4;j++)
        {
            res[i] += m.data[i][j] * v[j];
        }
    }
    if(res[3] != 0)
    {
        res[0] /= res[3];
        res[1] /= res[3];
        res[2] /= res[3];
    }
    return vec3(res[0], res[1], res[2]); 
}

vec3 transformPt(const mat4 &m, const vec3 &v)
{   
    float data[4] = {v.x, v.y, v.z, 1};
    return transform(m, data);
}

vec3 transformDir(const mat4 &m, const vec3 &v)
{
    float data[4] = {v.x, v.y, v.z, 0};
    return transform(m, data);
}

mat4 translation(const vec3 &t)
{
    return {{
        {1, 0, 0, t.x},
        {0, 1, 0, t.y},
        {0, 0, 1, t.z},
        {0, 0, 0, 1}
    }};
}
mat4 scaling(const vec3 &s)
{
    return {{
        {s.x, 0, 0, 0},
        {0, s.y, 0, 0},
        {0, 0, s.z, 0},
        {0, 0, 0, 1}
    }};
}

mat4 rotation(const vec3 &r)
{
    mat4 x = {{
        {1, 0, 0, 0},
        {0, cosf(r.x), -sinf(r.x), 0},
        {0, sinf(r.x), cosf(r.x), 0},
        {0, 0, 0, 1}
    }};
    mat4 y = {{
        {cosf(r.y), 0, sinf(r.y), 0},
        {0, 1, 0, 0},
        {-sinf(r.y), 0, cosf(r.y), 0},
        {0, 0, 0, 1}
    }};
    mat4 z = {{
        {cosf(r.z), -sinf(r.z), 0, 0},
        {sinf(r.z), cosf(r.z), 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
    return matmul(x, matmul(y, z));
}

enum ReflType {DIFF, REFL, REFR, FRES, LIGHT};

struct Material
{
    vec3 ks, kd, ka;
    float alpha, ior;
    ReflType type;
    Material(const vec3 &ks, const vec3 &kd, const vec3 &ka, const float &alpha, const float &ior, const ReflType &type):ks(ks),kd(kd),ka(ka),alpha(alpha), ior(ior),type(type)
    {}
    Material():alpha(32), ior(1.03),type(DIFF){}
};

class Shape
{
public:
    virtual bool intersect(const vec3 &ro, const vec3 &rd, float &t) const 
    {
        vec3 emin, emax;
        getExtents(emin, emax);
        vec3 bounds[2] = {emin, emax};
        return raybox(ro, rd, bounds, t);
    }
    virtual vec3 getDir(const vec3 &pt) const = 0;
    virtual vec3 getNorm(const vec3 &pt, const vec3 &rd) const = 0;
    virtual void getExtents(vec3 &emin, vec3 &emax) const = 0;
};

class Sphere: public Shape
{
public:
    Sphere(const vec3 &center, float rad):center(center), rad(rad), rad2(rad*rad)
    {}
    virtual bool intersect(const vec3 &ro, const vec3 &rd, float &t) const 
    {
        // if(!Shape::intersect(ro, rd, t)) return false;
        vec3 L = center - ro;
        float tca = dot(L, rd);
        if(tca < 0) return false;  // ray is facing the wrong way
        float d2 = dot(L, L) - tca * tca;
        if(d2 > rad2) return false;  // ray misses the sphere
        float thc = sqrt(rad2 - d2);
        float t0 = tca - thc;
        float t1 = tca + thc;
        if(t0 > t1) std::swap(t0, t1); 
        if(t0 < 0) t0 = t1; // we are inside the sphere
        if(t0 < 0 || t0 > t) return false; // the sphere is behind us or another shape is infront of it.
        t = t0;
        return true;
    }

    virtual vec3 getDir(const vec3 &pt) const 
    {
        return norm(center - pt);
    }

    virtual vec3 getNorm(const vec3 &pt, const vec3 &) const 
    {
        return norm(pt - center);
    }

    virtual void getExtents(vec3 &emin, vec3 &emax) const 
    {
        emin = center - rad;
        emax = center + rad;
    }

private:
    vec3 center;
    float rad, rad2;
};

class Triangle: public Shape
{
public:
    Triangle(const vec3 &a, const vec3 &b, const vec3 &c): a(a),b(b),c(c), center((a+b+c)/3.0f), ab(b-a),bc(c-b),ca(a-c),ba(a-b),cb(b-c),ac(c-a),normal(norm(cross(ab,ac)))
    {}
    virtual bool intersect(const vec3 &ro, const vec3 &rd, float &t) const
    {
        // if(!Shape::intersect(ro, rd, t)) return false;
        vec3 pvec = cross(rd, ac);
        float det = dot(ab, pvec);
        if(fabs(det) < std::numeric_limits<float>::epsilon()) return false;
        float idet = 1 / det;
        vec3 tvec = ro - a;
        float u = dot(tvec, pvec) * idet;
        if(u < 0 || u > 1) return false;
        vec3 qvec = cross(tvec, ab);
        float v = dot(rd, qvec) * idet;
        if(v < 0 || u + v > 1) return false;
        float t0 = dot(ac, qvec) * idet;
        if(t0 > t) return false;
        t = t0;
        return true;
    }

    virtual vec3 getDir(const vec3 &pt) const
    {
        return norm(center - pt);
    }

    virtual vec3 getNorm(const vec3 &, const vec3 &rd) const
    {
        return dot(rd, normal) < 0 ? normal : -normal;
    }

    virtual void getExtents(vec3 &emin, vec3 &emax) const
    {
        vec3 x = vec3(a.x, b.x, c.x);
        vec3 y = vec3(a.y, b.y, c.y);
        vec3 z = vec3(a.z, b.z, c.z);
        emin = vec3(min(x), min(y), min(z));
        emax = vec3(max(x), max(y), max(z));
    }

private:
    vec3 a, b, c, center;
    vec3 ab, bc, ca, ba, cb, ac;
    vec3 normal;
};

bool raybox(const vec3 &ro, const vec3 &rd, vec3 bounds[2], float &t)
{
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    int sign[3] = {rd.x < 0, rd.y < 0, rd.z < 0};
    vec3 invdir = 1/rd;
    tmin = (bounds[sign[0]].x - ro.x) * invdir.x;
    tmax = (bounds[1 - sign[0]].x - ro.x) * invdir.x;
    tymin = (bounds[sign[1]].y - ro.y) * invdir.y;
    tymax = (bounds[1 - sign[1]].y - ro.y) * invdir.y;

    if((tmin > tymax) || tymin > tmax)
    {
        return false;
    }
    if(tymin > tmin)
    {
        tmin = tymin;
    }
    if(tymax < tmax)
    {
        tmax = tymax;
    }

    tzmin = (bounds[sign[2]].z - ro.z) * invdir.z;
    tzmax = (bounds[1 - sign[2]].z - ro.z) * invdir.z;

    if((tmin > tzmax) || (tzmin > tmax))
    {
        return false;
    }

    if(tzmin > tmin)
    {
        tmin = tzmin;
    }
    if(tzmax < tmax)
    {
        tmax = tzmax;
    }
    float t0 = tmin;
    if(t0 < 0)
    {
        t0 = tmax;
        if(t0 < 0) return false;
    }
    if(t0 > t) return false;
    // t = t0;
    return true; 
}

class Object
{
public:
    Object(const Material &mat):emin(1e8,1e8,1e8),emax(-1e8,-1e8,-1e8),mat(mat)
    {}
    bool intersect(const vec3 &ro, const vec3 &rd, float &t, vec3 &pt, vec3 &n) const
    {
        vec3 bounds[2] = {emin, emax};
        // return raybox(ro, rd, bounds, t);
        if(!raybox(ro, rd, bounds, t))
        {
            return false;
        }
        int id = -1;
        for(size_t i = 0;i < shapes.size();i++)
        {
            if(shapes[i]->intersect(ro, rd, t))
            {
                id = i;
            }
        }
        if(id > -1)
        {
            pt = ro + rd * t;
            n = shapes[id]->getNorm(pt, rd);
            return true;
        }
        return false;
    }

    Material getcolor() const
    {
        return mat;
    }

    void toLight()
    {
        mat.type = LIGHT;
    }

    void setExtents(Shape * shape)
    {
        vec3 semin, semax;
        shape->getExtents(semin, semax);
        emin.x = std::min(emin.x, semin.x);
        emin.y = std::min(emin.y, semin.y);
        emin.z = std::min(emin.z, semin.z);

        emax.x = std::max(emax.x, semax.x);
        emax.y = std::max(emax.y, semax.y);
        emax.z = std::max(emax.z, semax.z);
    }

    void addShape(Shape * shape)
    {
        setExtents(shape);
        shapes.push_back(shape);
    }

    vec3 getDir(const vec3 &pt) const
    {
    //     std::uniform_real_distribution<float> fdist(0, 1);
    // auto randf = std::bind(fdist, gen);
        auto randx = std::bind(std::uniform_real_distribution<float>(emin.x, emax.x), gen);
        auto randy = std::bind(std::uniform_real_distribution<float>(emin.y, emax.y), gen);
        auto randz = std::bind(std::uniform_real_distribution<float>(emin.z, emax.z), gen);
        vec3 objpt = vec3(randx(), randy(), randz());
        return norm(objpt - pt);
        // vec3 dir;
        // for(size_t i = 0;i < shapes.size();i++)
        // {
        //     dir = dir + shapes[i]->getDir(pt);
        // }
        // dir = dir / (float)shapes.size();
        // return dir;
    }

    void addObj(const std::string &filename, const vec3 &tran, const vec3 &rota, const vec3 &scal)
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

        mat4 t = translation(tran);
        mat4 s = scaling(scal);
        mat4 r = rotation(rota);
        mat4 m = matmul(t, matmul(s, r));
        for(size_t s = 0; s < objshapes.size();s++)
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
                    verts[v] = transformPt(m, vec3(x, y, z));
                }
                addShape(new Triangle(verts[0], verts[1], verts[2]));
                index_offset += fv;
            }
        }
    }
private:
    std::vector<Shape*> shapes;
    vec3 emin, emax;
    Material mat;
};

SceneInfo options;
std::vector<Object> objects;

bool intersect(const vec3 &ro, const vec3 &rd, int &id, Material &mat, vec3 &pt, vec3 &n)
{
    float t = std::numeric_limits<float>::max();
    id = -1;
    for(size_t i = 0;i < objects.size();i++)
    {
        if(objects[i].intersect(ro, rd, t, pt, n))
        {
            id = i;
        }
    }
    if(id > -1)
    {
        mat = objects[id].getcolor();
        return true;
    }
    return false;
}

vec3 reflect(const vec3 &I, const vec3 &N)
{
    return N * (2 * dot(I, N)) - I;
}

vec3 refract(const vec3 &I, const vec3 &N, const float &ior)
{
    float cosi = clamp(dot(I, N), -1, 1);
    float etai = 1, etat = ior;
    vec3 n = N;
    if(cosi < 0)
    {
        cosi = -cosi;  
    } 
    else 
    {
        std::swap(etai, etat); 
        n = -N;
    }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi*cosi);
    return k < 0 ? vec3() : I * eta + n * (eta * cosi - sqrtf(k));
}

void fresnel(const vec3 &I, const vec3 &N, const float &ior, float &kr)
{
    float cosi = clamp(dot(I, N), -1, 1);
    float etai = 1, etat = ior;
    if (cosi > 0) {std::swap(etai, etat);}
    float sint = etai / etat * sqrtf(std::max(0.0f, 1 - cosi * cosi));
    if(sint >= 1)
    {
        kr = 1;
    }
    else
    {
        float cost = sqrtf(std::max(0.0f, 1 - sint * sint));
        cosi = fabsf(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cosi));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        kr = (Rs * Rs + Rp * Rp) / 2;
    }
}

vec3 trace(const vec3 &ro, const vec3 &rd, int depth)
{
    vec3 pt, n;
    Material mat;
    int id;
    if(--depth < 0 || !intersect(ro, rd, id, mat, pt, n))
    {
        return options.background;
    }

    if(mat.type == LIGHT)
    {
        return mat.kd;
    }

    vec3 kd;
    if(mat.type == REFL)
    {
        vec3 dir = norm(reflect(rd, n)); 
        kd = trace(pt + dir * 1e-4, dir, depth);
    }
    else if(mat.type == REFR)
    {
        vec3 dir = refract(rd, n, mat.ior);
        if(mag(dir) == 0)
        {
            kd = mat.kd;
        }
        else 
        {
            kd = trace(pt + dir * 1e-4, dir, depth);
        }
    }
    else if(mat.type == FRES)
    {
        vec3 tmpkd;
        float kr;
        fresnel(rd, n, mat.ior, kr);
        bool outside = dot(rd,n) < 0;
        vec3 bias = n * 1e-4;
        vec3 refrc;
        if(kr < 1)
        {
            vec3 refrdir = refract(rd, n, mat.ior);
            vec3 refrpt = outside ? pt - bias : pt + bias;
            refrc = trace(refrpt, refrdir, depth);
        }
        vec3 refldir = norm(reflect(rd, n));
        vec3 reflpt = outside ? pt + bias : pt - bias;
        vec3 reflc = trace(reflpt, refldir, depth);

        kd  = reflc * kr + refrc * (1 - kr);
    }
    else
    {
        kd = mat.kd;
    }

    vec3 v = -norm(rd);
    n = norm(n);
    vec3 finalColor;
    vec3 colours[objects.size()];
    bool hits[objects.size()];
    memset(hits, 0, sizeof(bool) * objects.size());
    for(size_t k = 0;k < options.samples;k++)
    {
        vec3 color = mat.ka * options.ambient;
        for(size_t i = 0;i < objects.size();i++)
        {
            Material objmat = objects[i].getcolor();
            vec3 objdir = objects[i].getDir(pt);
            int objid;
            vec3 objpt, objn;
            if(objmat.type != LIGHT || !intersect(pt+objdir*1e-4, objdir, objid, objmat, objpt, objn) || objid != (int)i) 
            {
                continue;
            };
            if(!hits[i])
            {
                hits[i] = true;
                vec3 l = norm(objdir);
                vec3 r = norm(n * (2 * dot(l, n)) - l); 
                colours[i] = (kd * std::max(dot(l, n), 0.0f) * objmat.kd) + (mat.ks * std::pow(std::max(dot(r, v), 0.0f), mat.alpha) * objmat.kd);
            }
            color = color + colours[i];
            
        }
        finalColor = finalColor + color / static_cast<float>(options.samples);
    }
    
    return finalColor;
}

void addShape(const Material &mat, Shape *shape)
{
    Object obj(mat);
    obj.addShape(shape);
    objects.push_back(obj);
}

void initObjects()
{
    std::uniform_real_distribution<float> fdist(0, 1);
    auto randf = std::bind(fdist, gen);

    addShape(Material(vec3(randf(), randf(), randf()),vec3(randf(), randf(), randf()), vec3(randf(), randf(), randf()), randf() * 128, randf() + 0.5, (ReflType)(randf() * 5)), new Sphere(vec3(0, 0, 0), 0.2));
    addShape(Material(vec3(1, 1, 1), vec3(1,1,1), vec3(1,1,1), 32, 1.03, LIGHT), new Sphere(vec3(0.0, 0.75, -0.5), 0.05));
}

std::string get_resources()
{
    std::string dir(__FILE__);
    std::string curdir = dir.substr(0, dir.find_last_of("\\/"));
    std::string parent = curdir.substr(0, curdir.find_last_of("\\/"));
    std::string result = parent + "/resources";
    return result;
}

int loadScene(const std::string &filename, SceneInfo &options, std::vector<Object> &objects)
{
    std::uniform_real_distribution<float> fdist(0, 1);
    auto randf = std::bind(fdist, gen);


    std::ifstream f(filename);
    if(!f.is_open())
    {
       return -1;
    }
    options.width = 224;
    options.height = 224;
    options.fov = 45.0f;
    options.background = vec3(randf(), randf(), randf());
    options.ambient = vec3(randf(), randf(), randf());
    options.samples = 16;
    std::string token;
    while(f >> token)
    {
        std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c){return std::tolower(c);});
        ReflType type = (ReflType)(randf() * 5);
        // ReflType type = DIFF;

        Object obj(Material(vec3(randf(), randf(), randf()),vec3(randf(), randf(), randf()), vec3(randf(), randf(), randf()), randf() * 128, randf() + 0.5, type));
        if(token == "width")
        {
            int w;
            f >> w;
            options.width = w;
        }
        else if(token == "height")
        {
            int h;
            f >> h;
            options.height = h;
        }
        else if(token == "fov")
        {
            float fov;
            f >> fov;
            options.fov = fov;
        }
        else if(token == "background")
        {
            vec3 background;
            f >> background;
            options.background = background;
        }
        else if(token == "ambient")
        {
            vec3 ambient;
            f >> ambient;
            options.ambient = ambient;
        }
        else if(token == "samples")
        {
            size_t samples;
            f >> samples;
            options.samples = samples;
        }
        else if(token == "sphere")
        {
            vec3 center;
            float rad;
            f >> center >> rad;
            obj.addShape(new Sphere(center, rad));
            objects.push_back(obj);
        }
        else if(token == "triangle")
        {
            vec3 a,b,c;
            f >> a >> b >> c;
            obj.addShape(new Triangle(a, b, c));
            objects.push_back(obj);
        }
        else if(token == "obj")
        {
            vec3 r,t,s;
            std::string filename;
            f >> filename >> t >> r >> s;
            r = vec3(radians(r.x), radians(r.y), radians(r.z));
            std::string path = get_resources() + "/objs/" + filename;
            obj.addObj(path, t, r, s);
            objects.push_back(obj);
        }
    }
    bool haslight = false;
    for(auto obj : objects)
    {
        if(obj.getcolor().type == LIGHT)
        {
            haslight = true;
            break;
        }
    }
    if(objects.size() > 0 && !haslight)
    {
        size_t idx = randf() * objects.size();
        objects[idx].toLight();
    }
    return 0;
}


vec3 *colors;

vec3 distributed_threaded_method(const vec3 &eye, float x, float y, float px, float py)
{
    std::uniform_real_distribution<float> fdist(-1, 1);
    auto randf = std::bind(fdist, gen);
    std::vector<std::thread> threads;
    for(size_t k = 0;k < options.samples;k++)
    {
        float dx = x + randf() * px;
        float dy = y + randf() * py;
        vec3 dir = norm(vec3(dx, dy, 1));
        threads.push_back(std::thread([&](vec3 eye, vec3 dir, size_t k){
            colors[k] = trace(eye, dir, 5);
        }, eye, dir, k));
    }

    vec3 color;
    for(size_t k = 0;k < options.samples;k++)
    {
        threads[k].join();
        color = color + colors[k] / static_cast<float>(options.samples);
    }
    return color;
}

vec3 distributed_single_method(const vec3 &eye, float x, float y, float px, float py)
{
    std::uniform_real_distribution<float> fdist(-1, 1);
    auto randf = std::bind(fdist, gen);
    vec3 color;
    for(size_t i = 0;i < options.samples;i++)
    {
        float dx = x + randf() * px;
        float dy = y + randf() * py;
        vec3 dir = norm(vec3(dx, dy, 1));
        color = color + trace(eye, dir, 5) / static_cast<float>(options.samples);
    }
    return color;
}

vec3 single_method(const vec3 &eye, float x, float y)
{
    vec3 dir = norm(vec3(x, y, 1));
    return trace(eye, dir, 5);
}

int main(int argc, char ** argv)
{
    gen.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    std::string scenename = get_resources() + "/scenes/basic.scene";
    if(argc >= 2)
    {
        scenename = std::string(argv[1]);
    }
    if(loadScene(scenename, options, objects) == -1)
    {
        perror(scenename.c_str());
        return -1;
    }

    float w = options.width, h = options.height;
    float pix_sizex = 1 / w;
    float pix_sizey = 1 / h;
    float scale = atan(radians(options.fov) * 0.5f);
    float aspect = w/h;

    colors = new vec3[options.samples];

    
    std::ofstream f("out.ppm");
    f << "P3\n" << options.width << " " << options.height << "\n255\n";
    
    vec3 eye(0, 0, -3);
    for(int i = 0;i < options.height;i++)
    {
        std::cerr << " Rows: " << i << "/" << options.height << "     \r" << std::flush;
        for(int j = 0;j < options.width;j++)
        {
            float x = (2.0f * (((j + 0.5f) / w) - 0.5f)) * scale * aspect;
            float y = (2.0f * (0.5f - ((i + 0.5f) / h))) * scale;
            vec3 color = distributed_single_method(eye, x, y, pix_sizex, pix_sizey);
            
            f << scaleValue(color.x) << " " << scaleValue(color.y) << " " << scaleValue(color.z) << " ";
        }
    }
    std::cerr << std::endl;
    delete [] colors;
    return 0;
}